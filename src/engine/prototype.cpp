#include "engine.hpp"
#include <list>
#include <mutex>


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
inline type::object skybox;

inline type::program shader;
inline type::font font;

inline spatial::matrix ortho;

inline spatial::matrix perspective;

inline spatial::position pos;
inline spatial::position camera;

inline spatial::vector mouse;

bool init = false;

time_t timestamp = time(NULL);
int frames = 0;
float fps = 0.0f;

void print(int x, int y, std::string text) {
    spatial::matrix position;
    position.identity();
    position.scale(0.8f);
    position.translate(x, y, 0);

    graphics->draw(text, font, shader, position, spatial::matrix(), ortho);
}

void print(int x, int y, spatial::vector vector) {
    for (int row = 0; row < 4; row++) {
        std::stringstream ss;
        ss << "[ ";
        for (int col = 0; col < 4; col++) {
            ss << vector.l[col] << (col < 3 ? ", " : " ");
        }
        ss << "]";
        print(x, y, ss.str());
    }
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

inline float prior_x;
inline float prior_y;

void freelook_start(const platform::input::event& ev) {
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_move(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "on_drag(" << ev.point.x << ", " << ev.point.y << ")";
    messages.add(ss.str());
    camera.rotate(ev.point.y - prior_y, prior_x - ev.point.x);
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_zoom(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "on_zoom";
    messages.add(ss.str());
    camera.move(ev.point.y);
}

void mouse_move(const platform::input::event& ev) {
    mouse = ev.point;
}

void prototype::on_startup() {
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

    assets->retrieve("objects/skybox.obj") >> format::parser::obj >> skybox;
    graphics->compile(skybox.children[0]);

    assets->retrieve("objects/untitled.obj") >> format::parser::obj >> poly;
    graphics->compile(poly.children[0]);

    audio->compile(sound);

    init = true;

    // Hook up the input handlers
    input->handler(platform::input::POINTER, platform::input::DOWN, &freelook_start, 2);
    input->handler(platform::input::POINTER, platform::input::DRAG, &freelook_move, 0);

    input->handler(platform::input::POINTER, platform::input::WHEEL, &freelook_zoom, 0);

    input->handler(platform::input::POINTER, platform::input::MOVE, &mouse_move, 0);
}

void prototype::on_resize() {
    glViewport(0, 0, width, height);
    ortho.ortho(0, width, 0, height);
    perspective.perspective(deg_to_radf(90), (float)width / (float)height, -1.0f, 1.0f);
}

void prototype::on_draw() {
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

    //graphics->draw(icon, shader, model, view, perspective);

    //graphics->draw(icon, shader, spatial::matrix(), view, perspective);

    graphics->draw(skybox.children[0], shader, spatial::matrix(), view, perspective);

    graphics->draw(poly.children[0], shader, spatial::matrix(), view, perspective);

    //print(100, 400, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    //print(100, 450, "0123456789 !@#$%^&*()_-=+<>,./?{[]}\|");

    print(30, height - 30, "MODEL");
    print(30, height - 60, model);

    print(30, height - 210, "VIEW");
    print(30, height - 240, view);

    print(30, height - 390, "MOUSE");
    print(30, height - 420, mouse);

    spatial::vector unprojected_projection;
    spatial::vector unprojected_ortho;

    unprojected_projection.unproject(mouse, spatial::matrix(), perspective, width, height);
    unprojected_ortho.unproject(mouse, spatial::matrix(), ortho, width, height);

    print(30, height - 450, unprojected_ortho);
    print(30, height - 480, unprojected_projection);

    print(600, height, messages);

    std::string value = utilities::type_cast<std::string>(fps);
    print(900, height - 30, std::string("FPS: ") + value);

    frames += 1;
    time_t now = time(NULL);
    if (timestamp != now) {
        timestamp = now;
        fps = frames;
        frames = 0;
    }

    graphics->flush();

}

void prototype::on_interval() {
    messages.add("on_proc");
}
