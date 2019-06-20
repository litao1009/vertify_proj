#ifdef CppLibPack_OpenGL_Support

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
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
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    auto gl_context = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(gl_context);

    gl3wInit();

    GLint ms;
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &ms);

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

    cv::imwrite("colorBuf.png", colorBuf);
    cv::imwrite("depthBuf.png", depthBuf);
    cv::imwrite("stencilBuf.png", stencilBuf);
    cv::imwrite("dsBuf.png", dsBuf);

    glfwDestroyWindow(gl_context);
    glfwTerminate();
}


#endif //CppLibPack_OpenGL_Support