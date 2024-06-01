extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <inttypes.h>
}

bool a_error_msg(const char *msg){printf("%s\n", msg); return false;}
bool audio_frame(const char* filename, int* awidth, int* aheight, unsigned char **adata){
    AVFormatContext *av_format_ctx = avformat_alloc_context();
    if(!av_format_ctx) a_error_msg("cannot initialize avformat");
    if(avformat_open_input(&av_format_ctx,filename, nullptr, nullptr) < 0)
        a_error_msg("cannot open audiofile data");
    int audio_stream_index = -1;
    AVCodecParameters* av_codec_par;
    const AVCodec* av_codec;
    for(int asx = 0; asx < av_format_ctx->nb_streams; asx++){
        auto streams = av_format_ctx->streams[asx];
        av_codec_par = streams->codecpar;
        av_codec = avcodec_find_decoder(av_codec_par->codec_id);
        if(!av_codec) continue;
        if(av_codec_par->codec_type == AVMEDIA_TYPE_AUDIO)
            audio_stream_index = asx; break;
    }
    if(audio_stream_index == -1) a_error_msg("could not find audio stream");
    AVCodecContext* av_codec_context = avcodec_alloc_context3(av_codec);
    if(!av_codec_context) a_error_msg("cannot initialize av_codec context");
    if(avcodec_parameters_to_context(av_codec_context, av_codec_par) < 0)
        a_error_msg("cannot parameterise context");
    if(avcodec_open2(av_codec_context, av_codec, nullptr) < 0)
        a_error_msg("cannot open av_codec file");
    
    int response;
    AVFrame* av_frame = av_frame_alloc();
    if(!av_frame) a_error_msg("cannot initialize av_frame");
    AVPacket* av_packet = av_packet_alloc();
    if(!av_packet) a_error_msg("cannot initialize av_packet");
    while(av_read_frame(av_format_ctx, av_packet) >= 0){
        if(av_packet->stream_index != audio_stream_index) continue;
        response = avcodec_send_packet(av_codec_context, av_packet);
        if(response < 0) a_error_msg(av_err2str(response));
        response = avcodec_receive_frame(av_codec_context, av_frame);
        if(response == AVERROR(EAGAIN) || response == AVERROR_EOF) continue;
        else if(response < 0) a_error_msg(av_err2str(response));
        av_packet_unref(av_packet);
        break;
    }
    
    uint8_t* data = new uint8_t [av_frame->width * av_frame->height * 4];
    SwsContext* sws_scale_ctx = sws_getContext(av_frame->width, av_frame->height, av_codec_context->pix_fmt, av_frame->width, av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if(!sws_scale_ctx) delete[] data; a_error_msg("failed to set sws scale");
    uint8_t* dest[4] = {data, nullptr, nullptr, nullptr};
    int dest_lines[4] = {av_frame->width * 4, 0, 0, 0};
    sws_scale(sws_scale_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_lines);
    sws_freeContext(sws_scale_ctx);
    
    *awidth = av_frame->width;
    *aheight = av_frame->height;
    *adata = data;
    
    avformat_close_input(&av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_context);
}
