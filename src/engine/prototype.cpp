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

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;

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

void print(int x, int y, const glm::mat4& matrix, int offset = 30) {
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

    std::vector<glm::vec4> list;

    list.push_back(glm::vec4(1.0f, 2.0f, 3.0f, 127.0f));
    list.push_back(glm::vec4(1.0f, 2.0f, 3.0f, 127.0f));
    list.push_back(glm::vec4(1.0f, 2.0f, 3.0f, 127.0f));

    std::ofstream output;
    output.open("c:\\projects\\output.txt");
    output.write((char *)list.data(), sizeof(glm::vec4) * list.size());
    output.close();
}

void prototype::on_resize() {
    glViewport(0, 0, width, height);
    ortho.ortho(0, width, 0, height);
    perspective.perspective(deg_to_radf(90), (float)width / (float)height, -1.0f, 1.0f);

    Projection = glm::perspective(glm::pi<float>() * 0.25f, (float)width / (float)height, 1.0f, 100.0f);
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
    frame.translate(400, 400, 0);

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

    glm::vec3 veye;
    veye.x = camera.eye.x;
    veye.y = camera.eye.y;
    veye.z = camera.eye.z;

    glm::vec3 vcenter;
    vcenter.x = camera.center.x;
    vcenter.y = camera.center.y;
    vcenter.z = camera.center.z;

    glm::vec3 vup;
    vup.x = camera.up.x;
    vup.y = camera.up.y;
    vup.z = camera.up.z;

    View = glm::lookAt(veye, vcenter, vup);

    //graphics->draw(icon, shader, model, view, perspective);

    //graphics->draw(icon, shader, spatial::matrix(), view, perspective);

    graphics->draw(skybox.children[0], shader, spatial::matrix(), view, perspective);

    graphics->draw(poly.children[0], shader, spatial::matrix(), view, perspective);
    //frame.scale(0.001);
    //graphics->draw(icon, shader, frame, view, perspective);

    //print(100, 400, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    //print(100, 450, "0123456789 !@#$%^&*()_-=+<>,./?{[]}\|");

    print(30, height - 30, "MODEL");
    print(30, height - 60, model);

    print(30, height - 210, "VIEW");
    print(30, height - 240, view);

    print(30, height - 390, "MOUSE");
    print(30, height - 420, mouse);

    //print(30, height - 520, "GLM::MAT4");
    //print(30, height - 550, View);

    spatial::vector unprojected_projection = mouse.unproject(view, perspective, width, height);
    spatial::vector unprojected_ortho = mouse.unproject(spatial::matrix(), ortho, width, height);

    auto relative = mouse;
    relative.x = relative.x;
    relative.y = height - relative.y;

    spatial::vector projected_ortho = relative.project(spatial::matrix(), spatial::matrix(), spatial::matrix());

    print(30, height - 450, projected_ortho);
    //print(30, height - 480, unprojected_projection);

    // temporary to test the math
    spatial::triangle t1;
    t1.vertices[0] = icon.vertices[0];
    t1.vertices[1] = icon.vertices[1];
    t1.vertices[2] = icon.vertices[2];

    spatial::triangle t2;
    t2.vertices[0] = icon.vertices[3];
    t2.vertices[1] = icon.vertices[4];
    t2.vertices[2] = icon.vertices[5];

    t1.project(frame, spatial::matrix(), spatial::matrix());
    t2.project(frame, spatial::matrix(), spatial::matrix());

    t1.vertices[0].coordinate.w = 1.0f;
    t1.vertices[1].coordinate.w = 1.0f;
    t1.vertices[2].coordinate.w = 1.0f;

    t2.vertices[0].coordinate.w = 1.0f;
    t2.vertices[1].coordinate.w = 1.0f;
    t2.vertices[2].coordinate.w = 1.0f;

    print(30, height - 520, "TRIANGLE 1");
    print(30, height - 550, t1.vertices[0].coordinate);
    print(30, height - 580, t1.vertices[1].coordinate);
    print(30, height - 610, t1.vertices[2].coordinate);

    print(30, height - 670, "TRIANGLE 2");
    print(30, height - 700, t2.vertices[0].coordinate);
    print(30, height - 730, t2.vertices[1].coordinate);
    print(30, height - 760, t2.vertices[2].coordinate);

    spatial::ray r1;

    r1.origin = projected_ortho;
    r1.terminus = projected_ortho;
    r1.origin.z = 100;
    r1.terminus.z = -100;

    //auto i1 = r1.intersection(t1);
    //print(30, height - 820, i1);

    if (r1.intersects(t1) || r1.intersects(t2)) {
        print(200, height - 390, "(HOVER OVER)");
    }

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
