#ifdef _WIN32
#define ENABLE_OGL_TEST
#elif __has_include(<GL/osmesa.h>)
#define ENABLE_OGL_TEST
#endif

#ifdef ENABLE_OGL_TEST

#include "gtest/gtest.h"
#include "glog/logging.h"


#ifdef _WIN32
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#else
#define GL_GLEXT_PROTOTYPES
#include "GL/osmesa.h"
#include "GL/glcorearb.h"
#endif

#include <assert.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

#define CHECK_ERROR err = glGetError();assert(err == 0);

TEST(opengl, triangle)
{
#ifdef _WIN32
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    auto gl_context = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(gl_context);

    gl3wInit();
#else

    cv::Mat frontBuf = cv::Mat::zeros(600, 800, CV_8UC4);

    int attribList[] =
    {
        OSMESA_FORMAT, OSMESA_BGRA,
        OSMESA_DEPTH_BITS, 24,
        OSMESA_STENCIL_BITS, 8,
        //OSMESA_ACCUM_BITS, 0,
        OSMESA_PROFILE, OSMESA_CORE_PROFILE,
        OSMESA_CONTEXT_MAJOR_VERSION, 3,
        //OSMESA_CONTEXT_MINOR_VERSION, 3,
        0
    };
    auto mesaCtx = OSMesaCreateContextAttribs(attribList, NULL);
    //auto mesaCtx = OSMesaCreateContextExt(OSMESA_RGBA, 32, 8, 0, nullptr);
    //auto mesaCtx = OSMesaCreateContext(OSMESA_BGRA, NULL);

    auto su = OSMesaMakeCurrent(mesaCtx, frontBuf.data, GL_UNSIGNED_BYTE, 800, 600);
    assert(su == GL_TRUE);
#endif

    GLint version[2];
    glGetIntegerv(GL_MAJOR_VERSION, &version[0]);
    auto err = glGetError();
    glGetIntegerv(GL_MINOR_VERSION, &version[1]);

    auto vertion_str = glGetString(GL_VERSION);
    auto vendor_str = glGetString(GL_VENDOR);
    auto renderer_str = glGetString(GL_RENDERER);
    auto shader_lang_str = glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint texture;
    glGenTextures(1, &texture);
    {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    auto chk = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(chk == GL_FRAMEBUFFER_COMPLETE);

    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLint maxLength = 0;
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
    maxLength = 4096;
    auto bufLength = 0;
    {
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if ( !success )
        {
            std::string str;
            str.resize(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &bufLength, str.data());
            FAIL() << str;
        }
    }

    auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        glShaderSource(fragShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragShader);

        GLint success;
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if ( !success )
        {
            std::string str;
            str.resize(maxLength);
            glGetShaderInfoLog(fragShader, maxLength, &bufLength, str.data());
            FAIL() << str;
        }
    }

    auto shaderProg = glCreateProgram();
    {
        glAttachShader(shaderProg, vertexShader);
        glAttachShader(shaderProg, fragShader);
        glLinkProgram(shaderProg);

        GLint success;
        glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
        if ( !success )
        {
            err = glGetError();
            std::string str;
            str.resize(maxLength);
            glGetProgramInfoLog(shaderProg, maxLength, &bufLength, str.data());
            FAIL() << str;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    CHECK_ERROR;

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    CHECK_ERROR;

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(.1f, .2f, .3f, .8f);
    glClearDepth(0.4);
    glClearStencil(0xf0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shaderProg);
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFinish();

    cv::Mat colorBuf = cv::Mat::zeros(600, 800, CV_8UC4);
    glReadPixels(0, 0, 800, 600, GL_BGRA, GL_UNSIGNED_BYTE, colorBuf.data);

    cv::Mat depthBuf = cv::Mat::zeros(600, 800, CV_32FC1);
    glReadPixels(0, 0, 800, 600, GL_DEPTH_COMPONENT, GL_FLOAT, depthBuf.data);

    cv::Mat stencilBuf = cv::Mat::zeros(600, 800, CV_8UC1);
    glReadPixels(0, 0, 800, 600, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencilBuf.data);

    cv::Mat dsBuf = cv::Mat::zeros(600, 800, CV_8UC4);
    glReadPixels(0, 0, 800, 600, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, dsBuf.data);

    
#if _WIN32
    //auto lnx_colorBuf = cv::imread("J:/test/lnx_colorBuf.png", cv::IMREAD_UNCHANGED);
    //auto lnx_depthBuf = cv::imread("J:/test/lnx_depthBuf.png", cv::IMREAD_UNCHANGED);
    //auto lnx_stencilBuf = cv::imread("J:/test/lnx_stencilBuf.png", cv::IMREAD_UNCHANGED);
    //auto lnx_dsBuf = cv::imread("J:/test/lnx_dsBuf.png", cv::IMREAD_UNCHANGED);

    cv::imwrite("win_colorBuf.png", colorBuf);
    cv::imwrite("win_depthBuf.png", depthBuf);
    cv::imwrite("win_stencilBuf.png", stencilBuf);
    cv::imwrite("win_dsBuf.png", dsBuf);
#else
    cv::imwrite("lnx_colorBuf.png", colorBuf);
    cv::imwrite("lnx_depthBuf.png", depthBuf);
    cv::imwrite("lnx_stencilBuf.png", stencilBuf);
    cv::imwrite("lnx_dsBuf.png", dsBuf);
#endif


#ifndef _WIN32
    cv::imwrite("frontBuf.png", frontBuf);
#endif

    OSMesaDestroyContext(mesaCtx);
    
}


#endif //ENABLE_OGL_TEST