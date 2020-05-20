#include "engine.hpp"


//=====================================================================================================

class message_container {
public:
    message_container(int entries = 40) {
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

    void clear() {
        lock.lock();
        data.clear();
        lock.unlock();
    }

    int limit;
private:

    std::list<std::string> data;
    std::mutex lock;
};

inline message_container text_input;
inline message_container text_data;
inline message_container text_events;

inline type::object box_input;
inline type::object box_data;
inline type::object box_events;


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
    position.scale(1.0f);
    position.translate(x, (graphics->height() - font.height()) - y, 0);

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

void print(int x, int y, spatial::matrix matrix) {
    for (int row = 0; row < 4; row++) {
        std::stringstream ss;
        ss << (row == 0 ? "[ [ " : "  [ ");
        for (int col = 0; col < 4; col++) {
            ss << matrix[row][col] << (col < 3 ? ", " : " ");
        }
        ss << (row == 3 ? "] ]" : "]");
        print(x, y + (font.leading() * row), ss.str());
    }
}

void print(int x, int y, const glm::mat4& matrix) {
    for (int row = 0; row < 4; row++) {
        std::stringstream ss;
        ss << (row == 0 ? "[ [ " : "  [ ");
        for (int col = 0; col < 4; col++) {
            ss << matrix[row][col] << (col < 3 ? ", " : " ");
        }
        ss << (row == 3 ? "] ]" : "]");
        print(x, y + (font.leading() * row), ss.str());
    }
}

void print(int x, int y, message_container &messages) {
    auto contents = messages.get();
    int line = y + (font.leading() * contents.size());
    for (auto message : contents) {
        print(x, y += font.leading(), message);
    }
}

void textbox(int x, int y, type::object &background, message_container& messages) {
    graphics->clip(graphics->height() - y, -((graphics->height() - background.height()) - y), -x, x + background.width());

    spatial::matrix position;
    position.identity();
    position.scale(1.0f);
    position.translate(x, (graphics->height() - background.height()) - y, 0);

    graphics->draw(background, shader, position, spatial::matrix(), ortho);

    print(x+10, y-30, messages);

    graphics->noclip();
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
    text_events.add(ss.str());
    camera.rotate(ev.point.y - prior_y, prior_x - ev.point.x);
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_zoom(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "on_zoom";
    text_events.add(ss.str());
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

    /// Load up the gui dependencies
    // TODO move these into the interface implementation
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> gui->shader.vertex;
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> gui->shader.fragment;
    graphics->compile(gui->shader);

    assets->retrieve("fonts/consolas-22.fnt") >> format::parser::fnt >> gui->font;
    graphics->compile(gui->font);

    /// Get the icon ready for drawing
    //assets->retrieve("drawable/marvin.png") >> format::parser::png >> icon.texture.map;

    box_events.texture.map.create(0, 0, 0, 80);
    box_events.quad(256, 512);
    box_events.xy_projection(0, 0, 256, 512);
    graphics->compile(box_events);

    icon.texture.map.create(0, 0, 0, 80);
    icon.quad(256, 256);
    icon.xy_projection(0, 0, 256, 256);
    graphics->compile(icon);

    assets->retrieve("fonts/consolas-22.fnt") >> format::parser::fnt >> font;
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

    // Create some gui elements
    gui->create(platform::interface::widget::type::button, 256, 256, 20, 0, 0, 80).position(200, 200).handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        std::stringstream ss;
        ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
        text_events.add(ss.str());
        print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap
    }, 1);

}

void prototype::on_resize() {
    graphics->geometry(width, height);

    ortho.ortho(0, width, 0, height);
    perspective.perspective(deg_to_radf(90), (float)width / (float)height, -1.0f, 1.0f);

    Projection = glm::perspective(glm::pi<float>() * 0.25f, (float)width / (float)height, 1.0f, 100.0f);

    gui->projection = ortho;
}

void prototype::on_draw() {
    graphics->clear();

    spatial::matrix frame;
    frame.identity();
    frame.translate(400, 400, 0);

    spatial::matrix center;
    center.identity();
    center.translate(width / 2, height / 2, 0);

    pos.rotate(0.0f, 1.0f);
    pos.move(0.02f);

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

    spatial::matrix box;
    //box.translate(5, 10, 0);

    graphics->draw(poly.children[0], shader, box, view, perspective);
    //frame.scale(0.001);
    //graphics->draw(icon, shader, frame, view, perspective);

    //print(100, 400, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    //print(100, 450, "0123456789 !@#$%^&*()_-=+<>,./?{[]}\|");

    print(30, 30, "MODEL");
    print(30, 30 + font.leading(), model);

    print(30, 210, "VIEW");
    print(30, 210 + font.leading(), view);

    print(30, 390, "MOUSE");
    print(30, 390 + font.leading(), mouse);

    textbox(600, 10, box_events, text_events);

    std::string value = utilities::type_cast<std::string>(fps);
    print(900, 30, std::string("FPS: ") + value);

    frames += 1;
    time_t now = time(NULL);
    if (timestamp != now) {
        timestamp = now;
        fps = frames;
        frames = 0;
    }

    graphics->draw(icon, shader, frame, spatial::matrix(), ortho);

    gui->draw();

    graphics->flush();
}

void prototype::on_interval() {
    text_events.add("on_proc");
}
