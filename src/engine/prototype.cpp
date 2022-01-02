#include "engine.hpp"

/*
TODO
scene object and gui management
time based animation and object manipulation
object automated motion and movement/camera constraints
input abstraction for user specified keybinds
network communication updated for HTTPS
cell shader
*/

/// <summary>
/// The input actions that are current active.  Allows for the abstraction of inputs to actions taken.
/// </summary>
class actions {

};

/// <summary>
/// Represents visual components of the current scene.
/// </summary>
class entity {
    class movement {
        /*
        Velocity calculation

        v = v0 + gt

        v0 initial velocity (m/s)
        t  time (s)
        g  gravitational acceleration (9.80665 m/s2)
        */

        float gravity;
    };

    type::object object;
    spatial::position target;
    spatial::position position;
};

/// <summary>
/// Organizes the resources, interface and actions that are currently active
/// as well as transations between them.
/// - Title
/// - Configuration
/// - Results
/// </summary>
class scene {

};

//=====================================================================================================

inline type::audio sound;

inline type::object icon;
inline type::object poly;
inline type::object skybox;
inline type::object wiggle;

inline type::object xAxis;
inline type::object yAxis;
inline type::object zAxis;

inline type::object ray;
inline type::object trail;

inline type::object sphere;
inline type::object visualized_bounds;
inline type::object ground;

inline spatial::quad bounds;

inline type::program shader;

inline spatial::matrix ortho;
inline spatial::matrix perspective;

inline spatial::position pos;
inline spatial::position camera;

inline std::list<spatial::position> projectiles;

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
int progress;

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
            ss << matrix[col][row] << (col < 3 ? ", " : " ");
        }
        ss << (row == 3 ? "] ]" : "]");
        gui->print(x, y + (gui->font.leading() * row), ss.str());
    }
}

void text_event(const std::string& text) {
    std::string output = text + "\n";
    gui->get<platform::interface::textbox>(events).content.add(output);
}

void progress_percentage() {
    static int count = 0;
    gui->get<platform::interface::progress>(progress).percentage = ++count > 100 ? 100 : count;
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
    ss << "freelook_move(" << ev.identifier << ")(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
    camera.pitch(ev.point.y - prior_y);
    camera.spin(prior_x - ev.point.x);
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_zoom(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "on_zoom(" << ev.travel << ")(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
    camera.move(ev.travel > 0 ? 0.1 : -0.1);
}

void mouse_move(const platform::input::event& ev) {
    mouse = ev.point;
    std::stringstream ss;
    ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
    text_event(ss.str());
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

bool camera_moving[4] = { false, false, false, false };
bool object_moving[6] = { false, false, false, false, false, false };
void keyboard_input(const platform::input::event& ev) {
    std::stringstream ss;
    ss << "key(" << ev.identifier << ")";
    text_event(ss.str());

    if (ev.gesture == platform::input::DOWN && gui->active() == false) {
        switch (ev.identifier) {
        case(37):
            object_moving[0] = true;
            break;
        case(38):
            object_moving[1] = true;
            break;
        case(39):
            object_moving[2] = true;
            break;
        case(40):
            object_moving[3] = true;
            break;
        case(188):
            object_moving[4] = true;
            break;
        case(190):
            object_moving[5] = true;
            break;
        case(83):
            camera_moving[0] = true;
            break;
        case(87):
            camera_moving[1] = true;
            break;
        case(65):
            camera_moving[2] = true;
            break;
        case(68):
            camera_moving[3] = true;
            break;
        }
    }

    if (ev.gesture == platform::input::UP && gui->active() == false) {
        switch (ev.identifier) {
        case(32):
            progress_percentage();
            projectiles.push_back(pos);
            if (projectiles.size() > 10) {
                projectiles.pop_front();
            }
            break;
        }
    }

    if (ev.gesture == platform::input::UP) {
        switch (ev.identifier) {
        case(37):
            object_moving[0] = false;
            break;
        case(38):
            object_moving[1] = false;
            break;
        case(39):
            object_moving[2] = false;
            break;
        case(40):
            object_moving[3] = false;
            break;
        case(188):
            object_moving[4] = false;
            break;
        case(190):
            object_moving[5] = false;
            break;
        case(83):
            camera_moving[0] = false;
            break;
        case(87):
            camera_moving[1] = false;
            break;
        case(65):
            camera_moving[2] = false;
            break;
        case(68):
            camera_moving[3] = false;
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

    bounds = spatial::quad(1, 1);

    // assets->retrieve("objects/wiggle.fbx") >> format::parser::fbx >> wiggle;
    // graphics->compile(wiggle.children[0]);

    assets->retrieve("raw/glados.wav") >> format::parser::wav >> sound;

    /// Load up the shaders
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> shader.vertex;
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> shader.fragment;
    graphics->compile(shader);

    //const char* vertex = shader.vertex.text.c_str();
    //const char* fragment = shader.fragment.text.c_str();

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

    /// <summary>
    /// Just some random objects to play around with
    /// </summary>
    ray = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
    ray.texture.map.create(1, 1, 255, 255, 0, 255);
    ray.xy_projection(0, 0, 1, 1);
    graphics->compile(ray);

    trail = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
    trail.texture.map.create(1, 1, 255, 0, 0, 255);
    trail.xy_projection(0, 0, 1, 1);
    graphics->compile(trail);

    sphere = spatial::sphere(30, 30);
    sphere.texture.map.create(1, 1, 255, 255, 255, 255);
    sphere.xy_projection(0, 0, 1, 1);
    graphics->compile(sphere);

    visualized_bounds = bounds;
    visualized_bounds.texture.map.create(1, 1, 255, 255, 255, 255);
    visualized_bounds.xy_projection(0, 0, 1, 1);
    graphics->compile(visualized_bounds);

    // TODO: find the bug that causes faults loading the same resources twice on android
    //assets->retrieve("fonts/consolas-22.fnt") >> format::parser::fnt >> font;
    //graphics->compile(font);

    assets->retrieve("objects/skybox.obj") >> format::parser::obj >> skybox;
    graphics->compile(skybox.children[0]);

    assets->retrieve("objects/untitled.obj") >> format::parser::obj >> poly;
    graphics->compile(poly.children[0]);

    assets->retrieve("objects/ground.obj") >> format::parser::obj >> ground;
    graphics->compile(ground.children[0]);

    audio->compile(sound);

    // Hook up the input handlers
    input->handler(platform::input::POINTER, platform::input::DOWN, &freelook_start, 2);
    input->handler(platform::input::POINTER, platform::input::DRAG, &freelook_move, 0);

    input->handler(platform::input::POINTER, platform::input::WHEEL, &freelook_zoom, 0);
    input->handler(platform::input::POINTER, platform::input::PINCH, &freelook_zoom, 0);

    input->handler(platform::input::POINTER, platform::input::MOVE, &mouse_move, 0);

    input->handler(platform::input::KEY, platform::input::DOWN, &keyboard_input, 0);
    input->handler(platform::input::KEY, platform::input::UP, &keyboard_input, 0);

    input->handler(platform::input::GAMEPAD, platform::input::DOWN, &gamepad_input, 0);
    input->handler(platform::input::GAMEPAD, platform::input::HELD, &gamepad_input, 0);
    input->handler(platform::input::GAMEPAD, platform::input::UP, &gamepad_input, 0);

    // Create some gui elements
    auto btn = gui->cast<platform::interface::button>(gui->create(platform::interface::widget::spec::button, 256, 256, 0, 0, 0, 80).position(20, 20).handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        std::stringstream ss;
        ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
        gui->print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap, don't intend to ever do this anyway
    }, 1)).handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
        std::stringstream ss;
        ss << "button_down(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
        audio->play(sound);
        client->connect();
    }, 1);

    events = gui->create(platform::interface::widget::spec::textbox, 512, 720, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 20).id;
    gui->get<platform::interface::textbox>(events).alignment = platform::interface::widget::positioning::bottom;

    textbox = gui->create(platform::interface::widget::spec::textbox, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 750).id;
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

    progress = gui->create(platform::interface::widget::spec::progress, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 780).id;

    server->handler([](platform::network::client* caller) {
        std::stringstream ss;
        std::string input(caller->input.begin(), caller->input.end());
        ss << "client_message(" << input << ")";
        text_event(ss.str());
        audio->play(sound);
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

    if (init) { // only adjust these after the initialization has occurred
        gui->get<platform::interface::textbox>(events).position(width - 512 - 20, 20);
        gui->get<platform::interface::textbox>(textbox).position(width - 512 - 20, 750);
        gui->get<platform::interface::progress>(progress).position(width - 512 - 20, 780);
    }
}

void prototype::on_draw() {
    graphics->clear();

    spatial::ray physical(pos.eye, spatial::vector(pos.eye.x, pos.eye.y - 1.0f, pos.eye.z));
    spatial::vector intersection = physical.intersection(ground.vertices);

    if (object_moving[0]) {
        pos.spin(1.0f);
    }
    if (object_moving[1]) {
        pos.move(0.1f);
    }
    if (object_moving[2]) {
        pos.spin(-1.0f);
    }
    if (object_moving[3]) {
        pos.move(-0.1f);
    }
    if (object_moving[4]) {
        pos.pitch(-1.0f);
    }
    if (object_moving[5]) {
        pos.pitch(1.0f);
    }

    if (camera_moving[0]) {
        camera.move(1);
    }
    if (camera_moving[1]) {
        camera.move(-1);
    }
    if (camera_moving[2]) {
        camera.strafe(1);
    }
    if (camera_moving[3]) {
        camera.strafe(-1);
    }

    spatial::matrix model = spatial::matrix().translate(pos.eye, pos.center, pos.up);
    spatial::matrix view = spatial::matrix().lookat(camera.eye, camera.center, camera.up);
    spatial::matrix box = spatial::matrix().translate(5, 10, 0);

    for (auto &projectile : projectiles) {
        spatial::matrix position = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
        trail = position.interpolate(spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2)));
        graphics->recompile(trail);
        
        graphics->draw(trail, shader, spatial::matrix(), view, perspective);

        projectile.move(0.4);

        spatial::matrix model = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
        graphics->draw(ray, shader, model, view, perspective);
    }

    {
        visualized_bounds = spatial::matrix().translate(pos.eye, pos.center, pos.up).interpolate(bounds);
        graphics->recompile(visualized_bounds);
        graphics->draw(visualized_bounds, shader, spatial::matrix(), view, perspective, platform::graphics::render::WIREFRAME);
    }

    graphics->draw(skybox, shader, spatial::matrix(), view, perspective);

    graphics->draw(ground, shader, spatial::matrix(), view, perspective, platform::graphics::render::NORMALS);

    graphics->draw(poly, shader, box, view, perspective, platform::graphics::render::NORMALS);

    graphics->draw(xAxis, shader, spatial::matrix(), view, perspective);
    graphics->draw(yAxis, shader, spatial::matrix(), view, perspective);
    graphics->draw(zAxis, shader, spatial::matrix(), view, perspective);

    graphics->draw(ray, shader, model, view, perspective);

    graphics->draw(sphere, shader, spatial::matrix(), view, perspective, platform::graphics::render::WIREFRAME);

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

    int pos_report = 630;
    int value_offset = 160;
    int entry = 1;

    gui->print(30, pos_report + gui->font.leading() * entry,   "POS EYE:");
    print(value_offset, pos_report + gui->font.leading() * entry++, pos.eye);
    gui->print(30, pos_report + (gui->font.leading() * entry), "POS CENTER:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), pos.center);
    gui->print(30, pos_report + (gui->font.leading() * entry), "POS UP:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), pos.up);
    gui->print(30, pos_report + (gui->font.leading() * entry), "PHY CENTER:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), physical.vertices[0]);
    gui->print(30, pos_report + (gui->font.leading() * entry), "PHY EYE:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), physical.vertices[1]);
    gui->print(30, pos_report + (gui->font.leading() * entry), "INTERSECTION:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), intersection);
    gui->print(30, pos_report + (gui->font.leading() * entry), "BOUNDS MAX:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), ground.max());
    gui->print(30, pos_report + (gui->font.leading() * entry), "BOUNDS MIN:");
    print(value_offset, pos_report + (gui->font.leading() * entry++), ground.min());

    spatial::matrix frame;
    frame.identity();
    frame.translate(20, graphics->height() - 20 - 256, 0);

    graphics->draw(icon, shader, frame, spatial::matrix(), ortho);

    {
        // This entire scope will render to the poly texture, every frame... which is unnecessary, just testing for performance, etc.
        auto scoped = graphics->target(poly);

        // just moving it a bit to move away from the edges
        spatial::matrix rendertotex;
        rendertotex.identity();
        rendertotex.translate(20, 20, 0);
        
        // orthographic view matrix relative to the target
        spatial::matrix ortho;
        ortho.ortho(0, poly.texture.map.properties.width, 0, poly.texture.map.properties.height);

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
