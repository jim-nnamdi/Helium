

#include <iostream>
#include <string>
#include <inttypes.h>
#include <GLFW/glfw3.h>

const int v_width = 640;
const int v_height = 480;
const char* v_title = "helium";

const char* INIT_ERR = "cannot initialize glfw modules";
const char* WIND_ERR = "cannot set up glfw windows";
const char* FRAME_ERR = "cannot load video frame info";

bool video_frame(const char *filename, int* vwidth, int* vheight, unsigned char* data);

int v_error_msg(const std::string& msg) { std::cerr << msg << std::endl; return false;}

int main(int argc, const char** argv){
    GLFWwindow* window;
    if(!glfwInit())
        v_error_msg(INIT_ERR);
    window = glfwCreateWindow(v_width, v_height, v_title, nullptr, nullptr);
    if(!window)
        v_error_msg(WIND_ERR);
    
    int framewidth, frameheight; unsigned char* framedata;
    if(video_frame("file_location",&framewidth, &frameheight, framedata))
        v_error_msg(FRAME_ERR);
    glfwMakeContextCurrent(window);
    GLuint vtexture;
    glGenTextures(1, &vtexture);
    glBindTexture(GL_TEXTURE_2D, vtexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGB, v_width, v_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, framedata);
    while(!glfwWindowShouldClose(window))
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0,0);
            glTexCoord2d(1,0); glVertex2i(framewidth,0);
            glTexCoord2d(1,1); glVertex2i(framewidth,frameheight);
            glTexCoord2d(0,1); glVertex2i(0,frameheight);
        glEnd();
        glfwSwapBuffers(window);
        glDisable(GL_TEXTURE_2D);
        glfwWaitEvents();
    return 0;
}