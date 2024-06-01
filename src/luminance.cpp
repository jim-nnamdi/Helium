#include <iostream>
#include <string>
#include <GLFW.glfw3.h>
#include "vluminance/luminance.hpp"

int main(){
    GLFWwindow* window;
    if(!glfwInit())
        Luminant::video_frame_msg("INIT_ERR");
    window = glfwCreateWindow(Luminant::getLwidth(), Luminant::getLheight(), "sample_title", nullptr, nullptr);
    if(!window)
        Luminant::video_frame_msg("WIND_ERR");
    
    int framewidth, frameheight; unsigned char* framedata;
    if(!Luminant::video_frame("file_location", &framewidth, &frameheight, framedata)) return false;
    glfwMakeContextCurrent(window);
    GLuint* vtex;
    glGenTextures(1, &vtex);
    glBindTextures(GL_TEXTURE_2D, vtex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_FILTER, GL_LINEAR);

    glTexImage2d(GL_TEXTURE_2D, 0 , GL_RGB, Luminant::getLwidth(), Luminant::getLheight(), 0 , GL_RGBA, GL_UNSIGNED_BYTES, framedata);
    while(!glfwShouldWindowClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0,0);
            glTexCoord2d(1,0); glVertex2i(framewidth, 0);
            glTexCoord2d(1,1); glVertex2i(framewidth, frameheight);
            glTexCoord2d(0,1); glVertex2i(0, frameheight);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glfwWaitEvents();
    }
    return 0;
}

