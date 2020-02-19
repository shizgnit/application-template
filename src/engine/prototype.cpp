#include "engine.hpp"
#include <list>
#include <mutex>

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

const GLfloat triangleVertices[] = {
        0.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
};


//=====================================================================================================

class message_container {
public:
    message_container(int entries = 20) {
        limit = entries;
    }

    void add(std::string message) {
        lock.lock();
        data.push_back(message);
        if (data.size() > limit) {
            data.pop_front();
        }
        lock.unlock();
    }

    std::vector<std::string> get() {
        std::vector<std::string> list;
        lock.lock();
        for (auto message: data) {
            list.push_back(message);
        }
        lock.unlock();
        return list;
    }

private:
    int limit;

    std::list<std::string> data;
    std::mutex lock;
} messages;

inline type::audio sound;

inline type::object icon;
inline type::object poly;

inline type::program shader;
inline type::font font;

inline spatial::matrix ortho;

inline spatial::matrix perspective;

inline spatial::position pos;
inline spatial::position camera;

bool init = false;

void print(int x, int y, std::string text) {
    spatial::matrix position;
    position.identity();
    position.scale(0.8f);
    position.translate(x, y, 0);

    graphics->draw(text, font, shader, position, spatial::matrix(), ortho);
}

void print(int x, int y, spatial::matrix matrix, int offset=30) {
    for (int row = 0; row < 4; row++) {
        std::stringstream ss;
        ss << (row == 0 ? "[ [ " : "  [ ");
        for (int col = 0; col < 4; col++) {
            ss << matrix[row][col] << (col < 3 ? ", " : " ");
        }
        ss << (row == 3 ? "] ]" : "]");
        print(x, y - (offset * row), ss.str());
    }

}

void print(int x, int y, message_container &messages, int offset=30) {
    auto contents = messages.get();

    int line = y + (offset * contents.size());
    for (auto message : contents) {
        print(x, y -= offset, message);
    }
}


float deg_to_radf(float deg) {
    return deg * (float)M_PI / 180.0f;
}

void application::on_startup() {
    simpleTriangleProgram = createProgram(glVertexShader, glFragmentShader);
    if (!simpleTriangleProgram) {
        return;
    }
    vPosition = glGetAttribLocation(simpleTriangleProgram, "vPosition");

    graphics->init();
    audio->init();

    assets->retrieve("raw/glados.wav") >> format::parser::wav >> sound;

    /// Load up the shaders
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> shader.vertex;
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> shader.fragment;
    graphics->compile(shader);

    /// Get the icon ready for drawing
    assets->retrieve("drawable/marvin.png") >> format::parser::png >> icon.texture.map;
    icon.quad(256, 256);
    icon.xy_projection(0, 0, 256, 256);
    graphics->compile(icon);

    assets->retrieve("fonts/arial.fnt") >> format::parser::fnt >> font;
    graphics->compile(font);

    assets->retrieve("objects/poly.obj") >> format::parser::obj >> poly;
    graphics->compile(poly);

    audio->compile(sound);

    init = true;
}

void application::on_resize() {
    glViewport(0, 0, width, height);
    ortho.ortho(0, width, 0, height);
    perspective.perspective(deg_to_radf(90), (float)width / (float)height, -1.0f, 1.0f);
}

void application::on_draw() {
    graphics->clear();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    //glUseProgram(simpleTriangleProgram);
    //glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, triangleVertices);
    //glEnableVertexAttribArray(vPosition);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    spatial::matrix frame;
    frame.identity();
    frame.translate(10, 10, 0);

    graphics->draw(icon, shader, frame, spatial::matrix(), ortho);

    spatial::matrix center;
    center.identity();
    center.translate(width / 2, height / 2, 0);

    pos.rotate(0.0f, 1.0f);
    pos.move(0.02f);

    //camera.move(4.0f);

    spatial::matrix model;
    model.identity();
    model.translate(pos.eye, pos.center, pos.up);

    spatial::matrix view;
    view.identity();
    view.lookat(camera.eye, camera.center, camera.up);

    graphics->draw(icon, shader, model, view, perspective);

    //print(100, 400, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    //print(100, 450, "0123456789 !@#$%^&*()_-=+<>,./?{[]}\|");

    print(30, height - 30, "MODEL");
    print(30, height - 60, model);

    print(30, height - 210, "VIEW");
    print(30, height - 240, view);

    print(450, height, messages);

    graphics->flush();
}

void application::on_proc() {
    messages.add("on_proc");
}

void application::on_press(float x, float y) {
    std::stringstream ss;
    ss << "on_press(" << x << ", " << y << ")";
    messages.add(ss.str());
}
void application::on_release(float x, float y) {
    std::stringstream ss;
    ss << "on_release(" << x << ", " << y << ")";
    messages.add(ss.str());
}
void application::on_move(long int x, long int y) {
    std::stringstream ss;
    ss << "on_move(" << x << ", " << y << ")";
    messages.add(ss.str());
}
void application::on_drag(float x, float y) {
    std::stringstream ss;
    ss << "on_drag(" << x << ", " << y << ")";
    messages.add(ss.str());
}
void application::on_scale(float x, float y, float z) {
    std::stringstream ss;
    ss << "on_scale(" << x << ", " << y << ", " << z << ")";
    messages.add(ss.str());
}
void application::on_zoom_in() {
    std::stringstream ss;
    ss << "on_zoom_in";
    messages.add(ss.str());
    camera.move(10.0f);
}

void application::on_zoom_out() {
    std::stringstream ss;
    ss << "on_zoom_out";
    messages.add(ss.str());
    camera.move(-10.0f);
}

void application::on_key_down(int key) {
    std::stringstream ss;
    ss << "on_key_down(" << key << ")";
    messages.add(ss.str());
}
void application::on_key_up(int key) {
    std::stringstream ss;
    ss << "on_key_up(" << key << ")";
    messages.add(ss.str());
}

