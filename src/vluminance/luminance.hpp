#include <iostream>

class Luminant{
    private: 
        int lwidth;
        int lheight;
    public: 
        Luminant(): lwidth(0), lheight(0){}
        Luminant(int vf_width, int vf_height){ lwidth = vf_width; lheight = vf_height;}
        bool video_frame(const char* filename, int* vf_width, int* vf_height, unsigned char * data);
        int video_frame_msg(const char* msg) {std::cerr << msg << std::endl; return false;}
        int getLwidth()  {return lwidth;}
        int getLheight() {return lheight;}
};