#include "engine.hpp"

//=====================================================================================================

inline type::audio sound;

inline type::object icon;
inline type::object poly;
inline type::object skybox;

inline type::object xAxis;
inline type::object yAxis;
inline type::object zAxis;

inline type::program shader;
//inline type::font font;

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

int events;
int textbox;

void print(int x, int y, spatial::vector vector) {
    for (int row = 0; row < 4; row++) {
        std::stringstream ss;
        ss << "[ ";
        for (int col = 0; col < 4; col++) {
            ss << vector.l[col] << (col < 3 ? ", " : " ");
        }
        ss << "]";
        gui->print(x, y, ss.str());
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
        gui->print(x, y + (gui->font.leading() * row), ss.str());
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
        gui->print(x, y + (gui->font.leading() * row), ss.str());
    }
}

void text_event(const std::string& text) {
    std::string output = text + "\n";
    gui->get<platform::interface::textbox>(events).content.add(output);
}

inline float prior_x;
inline float prior_y;

void freelook_start(const platform::input::event& ev) {
    prior_x = ev.point.x;
    prior_y = ev.point.y;
    std::stringstream ss;
    ss << "freelook_start(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
}

void freelook_move(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "freelook_move(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
    camera.rotate(ev.point.y - prior_y, prior_x - ev.point.x);
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_zoom(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "on_zoom: " << ev.point.y;
    text_event(ss.str());
    camera.move(ev.point.y * 2);
}

void mouse_move(const platform::input::event& ev) {
    mouse = ev.point;
    std::stringstream ss;
    ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

bool moving[4] = { false, false, false, false };
void keyboard_input(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "key(" << ev.identifier << ")";
    text_event(ss.str());

    if (ev.gesture == platform::input::DOWN) {
        switch (ev.identifier) {
        case(83):
            moving[0] = true;
            break;
        case(87):
            moving[1] = true;
            break;
        case(65):
            moving[2] = true;
            break;
        case(68):
            moving[3] = true;
            break;
        }
    }

    if (ev.gesture == platform::input::UP) {
        switch (ev.identifier) {
        case(83):
            moving[0] = false;
            break;
        case(87):
            moving[1] = false;
            break;
        case(65):
            moving[2] = false;
            break;
        case(68):
            moving[3] = false;
            break;
        }
    }
}

void gamepad_input(const platform::input::event& ev) {
    std::stringstream ss;
    if (ev.gesture == platform::input::DOWN || ev.gesture == platform::input::HELD) {
        ss << "button_down(" << ev.identifier << ")";
    }
    if (ev.gesture == platform::input::UP) {
        ss << "button_up(" << ev.identifier << ")";
    }
    text_event(ss.str());
}

void prototype::on_startup() {
    graphics->init();
    audio->init();

    assets->retrieve("raw/glados.wav") >> format::parser::wav >> sound;

    /// Load up the shaders
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> shader.vertex;
    const char* vert = shader.vertex.text.c_str();
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> shader.fragment;
    const char* frag = shader.fragment.text.c_str();
    graphics->compile(shader);

    /// Load up the gui dependencies
    // TODO move these into the interface implementation
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> gui->shader.vertex;
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> gui->shader.fragment;
    graphics->compile(gui->shader);

    assets->retrieve("fonts/consolas-22.fnt") >> format::parser::fnt >> gui->font;
    graphics->compile(gui->font);

    /// Get the icon ready for drawing
    icon = spatial::quad(256, 256);
    assets->retrieve("drawable/marvin.png") >> format::parser::png >> icon.texture.map;
    icon.xy_projection(0, 0, 256, 256);
    graphics->compile(icon);

    /// Setup the axis
    xAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(2.0, 0.0, 0.0));
    xAxis.texture.map.create(1, 1, 255, 0, 0, 255);
    xAxis.xy_projection(0, 0, 1, 1);
    graphics->compile(xAxis);
    yAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 2.0, 0.0));
    yAxis.texture.map.create(1, 1, 0, 255, 0, 255);
    yAxis.xy_projection(0, 0, 1, 1);
    graphics->compile(yAxis);
    zAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 0.0, 2.0));
    zAxis.texture.map.create(1, 1, 0, 0, 255, 255);
    zAxis.xy_projection(0, 0, 1, 1);
    graphics->compile(zAxis);

    // TODO: find the bug that causes faults loading the same resources twice on android
    //assets->retrieve("fonts/consolas-22.fnt") >> format::parser::fnt >> font;
    //graphics->compile(font);

    assets->retrieve("objects/skybox.obj") >> format::parser::obj >> skybox;
    graphics->compile(skybox.children[0]);

    assets->retrieve("objects/untitled.obj") >> format::parser::obj >> poly;
    graphics->compile(poly.children[0]);

    //audio->compile(sound);

    // Hook up the input handlers
    input->handler(platform::input::POINTER, platform::input::DOWN, &freelook_start, 2);
    input->handler(platform::input::POINTER, platform::input::DRAG, &freelook_move, 0);

    input->handler(platform::input::POINTER, platform::input::WHEEL, &freelook_zoom, 0);

    input->handler(platform::input::POINTER, platform::input::MOVE, &mouse_move, 0);

    input->handler(platform::input::KEY, platform::input::DOWN, &keyboard_input, 0);
    input->handler(platform::input::KEY, platform::input::UP, &keyboard_input, 0);

    input->handler(platform::input::GAMEPAD, platform::input::DOWN, &gamepad_input, 0);
    input->handler(platform::input::GAMEPAD, platform::input::HELD, &gamepad_input, 0);
    input->handler(platform::input::GAMEPAD, platform::input::UP, &gamepad_input, 0);

    // Create some gui elements
    auto btn = gui->cast<platform::interface::button>(gui->create(platform::interface::widget::type::button, 256, 256, 0, 0, 0, 80).position(20, 20).handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        std::stringstream ss;
        ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
        gui->print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap, don't intend to ever do this anyway
    }, 1)).handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
        std::stringstream ss;
        ss << "button_down(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
        client->connect();
     }, 1);

    events = gui->create(platform::interface::widget::type::textbox, 512, 720, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 20).id;
    gui->get<platform::interface::textbox>(events).alignment = platform::interface::widget::positioning::bottom;

    textbox = gui->create(platform::interface::widget::type::textbox, 512, 60, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 760).id;
    gui->get<platform::interface::textbox>(textbox).selectable = true;
    gui->get<platform::interface::textbox>(textbox).handler(platform::input::KEY, platform::input::DOWN, [](const platform::input::event& ev) {
        std::stringstream ss;
        std::vector<std::string> content;
        switch (ev.identifier) {
        case(8): // Backspace to remove a character
            gui->get<platform::interface::textbox>(textbox).content.remove(1);
            break;
        case(13): // Enter to submit
            content = gui->get<platform::interface::textbox>(textbox).content.get();
            if (content.size()) {
                ss << "text_submit(" << content[0] << ")";
                text_event(ss.str());
                gui->get<platform::interface::textbox>(textbox).content.remove(-1);
            }
            break;
        default: // Every other printable gets added to the contents
            gui->get<platform::interface::textbox>(textbox).content.append(input->printable(ev.identifier));
        };
    });

    server->handler([](platform::network::client* caller) {
        std::stringstream ss;
        std::string input(caller->input.begin(), caller->input.end());
        ss << "client_message(" << input << ")";
        text_event(ss.str());
    });

    server->start();

    init = true;
}

void prototype::on_resize() {
    graphics->geometry(width, height);

    ortho.ortho(0, width, 0, height);
    perspective.perspective(90 * (float)M_PI / 180.0f, (float)width / (float)height, -1.0f, 1.0f);

    Projection = glm::perspective(glm::pi<float>() * 0.25f, (float)width / (float)height, 1.0f, 100.0f);

    gui->projection = ortho;

    if (init) {
        gui->get<platform::interface::textbox>(events).position(width - 512 - 20, 20);
    }
}

void prototype::on_draw() {
    graphics->clear();

    spatial::matrix center;
    center.identity();
    center.translate(width / 2, height / 2, 0);

    pos.rotate(0.0f, 1.0f);

    //static float move = 0.02f;
    //move -= 0.02f;
    //pos.move(move);

    pos.move(0.02f);

    if (moving[0]) {
        camera.move(1);
    }
    if (moving[1]) {
        camera.move(-1);
    }
    if (moving[2]) {
        camera.strafe(1);
    }
    if (moving[3]) {
        camera.strafe(-1);
    }

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
    box.translate(5, 10, 0);

    graphics->draw(poly.children[0], shader, box, view, perspective);
    //frame.scale(0.001);
    //graphics->draw(icon, shader, frame, view, perspective);

    graphics->draw(xAxis, shader, spatial::matrix(), view, perspective);
    graphics->draw(yAxis, shader, spatial::matrix(), view, perspective);
    graphics->draw(zAxis, shader, spatial::matrix(), view, perspective);

    //print(100, 400, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    //print(100, 450, "0123456789 !@#$%^&*()_-=+<>,./?{[]}\|");

    gui->print(30, 330, "MODEL");
    print(30, 330 + gui->font.leading(), model);

    gui->print(30, 460, "VIEW");
    print(30, 460 + gui->font.leading(), view);

    gui->print(30, 590, "MOUSE");
    print(30, 590 + gui->font.leading(), mouse);

    //textbox(600, 10, box_events, text_events);
    //gui->get<platform::interface::textbox>(textbox).content.add(utilities::type_cast<std::string>(time(NULL)));

    std::string value = utilities::type_cast<std::string>(fps);
    gui->print(30, 300, std::string("FPS: ") + value);

    spatial::matrix frame;
    frame.identity();
    frame.translate(20, graphics->height() - 20 - 256, 0);

    graphics->draw(icon, shader, frame, spatial::matrix(), ortho);

    {
        auto scope = graphics->target(poly.children[0]);
        spatial::matrix rendertotex;
        rendertotex.identity();
        rendertotex.translate(20, 20, 0);
        graphics->draw(icon, shader, rendertotex, spatial::matrix(), ortho);
    }

    frames += 1;
    time_t now = time(NULL);
    if (timestamp != now) {
        timestamp = now;
        fps = frames;
        frames = 0;
    }

    gui->draw();

    graphics->flush();
}

void prototype::on_interval() {
    text_event("on_proc");
}
