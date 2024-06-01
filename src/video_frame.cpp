extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <inttypes.h>
    #include <stdio.h>
}

bool v_error_msg(const char* msg) {printf("%s\n", msg); return false;}
bool video_frame(const char *filename, int* vwidth, int* vheight, unsigned char** vdata){
    AVFormatContext* av_format_ctx = avformat_alloc_context();
    if(!av_format_ctx) v_error_msg("cannot initialize avformat context");
    if(avformat_open_input(&av_format_ctx, filename, nullptr,nullptr) < 0)
        v_error_msg("cannot open video file properly");
    int video_stream_index = -1;
    const AVCodec* avcodec;
    AVCodecParameters* avcodec_par;
    for(int vsx = 0; vsx < av_format_ctx->nb_streams; vsx++){
        auto streams = av_format_ctx->streams[vsx];
        avcodec_par = streams->codecpar;
        avcodec = avcodec_find_decoder(avcodec_par->codec_id);
        if(!avcodec) continue;
        if(avcodec_par->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = vsx; break;
    }
    if(video_stream_index == -1) v_error_msg("no video streams available");
    AVCodecContext* avcodec_ctx = avcodec_alloc_context3(avcodec);
    if(!avcodec_ctx) v_error_msg("cannot initialize avcodec context");
    if(avcodec_parameters_to_context(avcodec_ctx, avcodec_par) < 0)
        v_error_msg("cannot parameterize avcodec context");
    if(avcodec_open2(avcodec_ctx, avcodec, nullptr) < 0)
        v_error_msg("cannot fetch file from context");
    
    int response;
    AVFrame* av_frame = av_frame_alloc();
    if(!av_frame) v_error_msg("could not initialize av_frame");
    AVPacket* av_packet = av_packet_alloc();
    if(!av_packet) v_error_msg("could not initialize av packet");
    while(av_read_frame(av_format_ctx, av_packet) >= 0) {
        if(av_packet->stream_index != video_stream_index) continue;
        response = avcodec_send_packet(avcodec_ctx, av_packet);
        if(response < 0) v_error_msg("cannot fetch video packets");
        response = avcodec_receive_frame(avcodec_ctx, av_frame);
        if(response == AVERROR(EAGAIN) || response == AVERROR_EOF) continue;
        else if(response < 0) v_error_msg(av_err2str(response));
        av_packet_unref(av_packet);
        break;
    }
    if (av_frame->format != AV_PIX_FMT_YUV420P)
        return v_error_msg("Unsupported pixel format");
    uint8_t * data = new uint8_t[av_frame->width * av_frame->height * 4];
    SwsContext* sws_scale_ctx = sws_getContext(av_frame->width, av_frame->height,avcodec_ctx->pix_fmt, av_frame->width, av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if(!sws_scale_ctx)
        v_error_msg("cannot initialize sws scale");
    uint8_t* dest[4] = {data, nullptr, nullptr, nullptr};
    int dest_ls[4] = {av_frame->width * 4, 0,0,0};
    sws_scale(sws_scale_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_ls);
    sws_freeContext(sws_scale_ctx);

    *vwidth = av_frame->width;
    *vheight = av_frame->height;
    *vdata = data;
    
    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&avcodec_ctx);
    return true;
}
