#include "engine.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const char glVertexShader[] =
"attribute vec4 vPosition;\n"
"void main()\n"
"{\n"
"  gl_Position = vPosition;\n"
"}\n";

static const char glFragmentShader[] =
"precision mediump float;\n"
"void main()\n"
"{\n"
"  gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
"}\n";

GLuint loadShader(GLenum shaderType, const char* shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*)malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    //LOGE("Could not Compile Shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
    {
        return 0;
    }
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        return 0;
    }
    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*)malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    //LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint simpleTriangleProgram;
GLuint vPosition;

inline format::wav sound;

void application::on_startup() {
    simpleTriangleProgram = createProgram(glVertexShader, glFragmentShader);
    if (!simpleTriangleProgram) {
        return;
    }
    vPosition = glGetAttribLocation(simpleTriangleProgram, "vPosition");
    glViewport(0, 0, width, height);

    char buffer[500000];

    assets->retrieve("raw/glados.wav") >> sound;

    audio->init(4);

    audio->compile(sound);
}

void application::on_resize() {
    glViewport(0, 0, width, height);
}

const GLfloat triangleVertices[] = {
        0.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
};

void application::on_draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(simpleTriangleProgram);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, triangleVertices);
    glEnableVertexAttribArray(vPosition);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFlush();
}

void application::on_proc() {}

void application::on_touch_press(float x, float y) {}
void application::on_touch_release(float x, float y) {
    audio->play(sound);
}
void application::on_touch_drag(float x, float y) {}
void application::on_touch_scale(float x, float y, float z) {}
void application::on_touch_zoom_in() {}
void application::on_touch_zoom_out() {}

void application::on_key_down(int key) {}
void application::on_key_up(int key) {}

void application::on_mouse_move(long int x, long int y) {}


/// Just some tests for newer syntax on the different compilers
auto test_return() {
    return std::pair<int, int>({ 2, 10 });
}

void test_capabilities() {
    int something = 2;
    auto [foo, bar] = test_return();

    auto data = std::map<int, int>({ { 103, 103 } });

    for (const auto& [left, right] : data) {
        int x = left + right;
    }
}
