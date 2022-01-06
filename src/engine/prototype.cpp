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

int events;
int textbox;
int progress;

void text_event(const std::string& text) {
    for (auto line : utilities::tokenize(text)) {
        std::string output = line + "\n";
        gui->get<platform::interface::textbox>(events).content.add(output);
    }
}

void progress_percentage() {
    static int count = 0;
    gui->get<platform::interface::progress>(progress).percentage = ++count > 100 ? 100 : count;
}

typedef std::string label_t;
typedef std::variant<double, int, std::string, spatial::vector> value_t;

/// <summary>
/// Represents visual components of the current scene.
/// </summary>
class entity {
public:
    static int id() {
        static int count = 0;
        return count++;
    }

    entity(type::object* instance) {
        object = instance;
    }

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

    type::object *object;

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
class scenes {
public:
    class scene {
    public:
        scene() {}
        virtual ~scene() {}

        std::map<label_t, value_t> variables;
        std::map<int, entity> entities;
        std::vector<platform::interface::widget> widgets;

        virtual void load() {}
        virtual void start() {}
        virtual void run() {}
        virtual void stop() {}

        /// <summary>
        /// Scene specific event handlers
        /// </summary>
        virtual void freelook_start(const platform::input::event& ev) {}
        virtual void freelook_move(const platform::input::event& ev) {}
        virtual void freelook_zoom(const platform::input::event& ev) {}
        virtual void mouse_move(const platform::input::event& ev) {}
        virtual void keyboard_input(const platform::input::event& ev) {}
        virtual void gamepad_input(const platform::input::event& ev) {}

        void set(std::string entity, label_t label, value_t value) {

        }

        bool loaded = false;
        bool displayed = false;

        spatial::position camera;
    };

    static scenes& global() {
        static scenes singleton;
        return singleton;
    }

    static scene& current() {
        static scene empty;
        std::string active = global().active;
        if (active.empty() || global().instances.find(active) == global().instances.end()) {
            return empty;
        }
        return *global().instances[active];
    }

    void transition(std::string name) {
        std::thread([] {
            
        }).detach();

        if (instances.find(name) != instances.end()) {

        }
        if (instances[name]->loaded == false) {
            instances[name]->load();
        }
        std::string current = active;
        active = name;
        instances[current]->stop();
        instances[name]->start();
        instances[name]->run();
        instances[current]->start();
        active = current;
    }

    value_t get(label_t label) {
        return variables[label];
    }

    bool flag(label_t label) {
        if (variables.find(label) == variables.end()) {
            return false;
        }
        auto variable = variables[label];
        if (std::holds_alternative<int>(variable)) {
            return std::get<int>(variable) != 0;
        }
        if (std::holds_alternative<double>(variable)) {
            return std::get<double>(variable) > 0.0f;
        }
        if (std::holds_alternative<std::string>(variable)) {
            return std::get<std::string>(variable).empty();
        }
        if (std::holds_alternative<spatial::vector>(variable)) {
            return std::get<spatial::vector>(variable).value();
        }
        return false;
    }

    void set(label_t label, value_t value) {
        variables[label] = value;
    }

    void load(std::string type, std::string name) {
        if (type == "audio") {
            assets->retrieve("raw/" + name + ".wav") >> format::parser::wav >> sounds[name];
        }
        if (type == "shader") {
            assets->retrieve("shaders/" + name + ".vert") >> format::parser::vert >> shaders[name].vertex;
            assets->retrieve("shaders/" + name + ".frag") >> format::parser::frag >> shaders[name].fragment;
            graphics->compile(shaders[name]);
        }
        if (type == "font") {
            assets->retrieve("fonts/" + name + ".fnt") >> format::parser::fnt >> fonts[name];
            graphics->compile(fonts[name]);
        }
        if (type == "object") {
            if (name.substr(name.size() - 4, 4) == ".fbx") {
                assets->retrieve("objects/" + name) >> format::parser::fbx >> objects[name];
            }
            else if (name.substr(name.size() - 4, 4) == ".obj") {
                assets->retrieve("objects/" + name) >> format::parser::obj >> objects[name];
            }
            else {
                assets->retrieve("objects/" + name + ".obj") >> format::parser::obj >> objects[name];
            }
            graphics->compile(objects[name]);
        }
        if (type == "entity") {
            assets->retrieve("objects/" + name + ".obj") >> format::parser::obj >> objects[name];
            graphics->compile(objects[name]);
        }
    }

    static void freelook_start(const platform::input::event& ev) { scenes::current().freelook_start(ev); }
    static void freelook_move(const platform::input::event& ev) { scenes::current().freelook_move(ev); }
    static void freelook_zoom(const platform::input::event& ev) { scenes::current().freelook_zoom(ev); }
    static void mouse_move(const platform::input::event& ev) { scenes::current().mouse_move(ev); }
    static void keyboard_input(const platform::input::event& ev) { scenes::current().keyboard_input(ev); }
    static void gamepad_input(const platform::input::event& ev) { scenes::current().gamepad_input(ev); }

    std::map<std::string, type::font> fonts;
    std::map<std::string, type::program> shaders;
    std::map<std::string, type::audio> sounds;
    std::map<std::string, type::object> objects;

    std::map<label_t, value_t> variables;

    std::map<std::string, scene*> instances;
    std::string active;
};

/// <summary>
/// The input actions that are current active.  Allows for the abstraction of inputs to actions taken.
/// </summary>
class actions {
  public:
    typedef std::vector<value_t> parameters_t;
    typedef value_t(*callback_t)(parameters_t);

    static actions& instance() {
        static actions singleton;
        return singleton;
    }

    value_t call(const std::string& input) {
        auto tokens = utilities::tokenize(input, " ");
        if (tokens.empty() || commands.find(tokens[0]) == commands.end()) {
            std::vector<std::string> list;
            for (auto command : commands) {
                list.push_back(command.first);
            }
            std::stringstream ss;
            ss << "commands: " << utilities::join(", ", list);
            text_event(ss.str());
            return 0;
        }
        parameters_t params;

        std::string command;
        std::string buffer;

        for (auto token : tokens) {
            // First token is always the command to execute
            if (command.empty()) {
                command = token;
            }
            // String processing in quotes can span multiple tokens
            else if (buffer.empty() == false) {
                buffer.append(" ");
                if (token.back() == '\"') {
                    token.erase(token.size() - 1);
                    buffer.append(token);
                    params.push_back(buffer);
                    buffer.clear();
                }
                else {
                    buffer.append(token);
                }
            }
            else if (token.front() == '\"') {
                token.erase(0, 1);
                if (token.back() == '\"') {
                    token.erase(token.size() - 1);
                    params.push_back(token);
                }
                else {
                    buffer = token;
                }
            }
            // Spatial vector data
            else if (token.front() == '(' && token.back() == ')') {
                token.erase(0, 1);
                token.erase(token.size() - 1);
                auto axis = utilities::tokenize(token, ",");
                spatial::vector vector = {
                    axis.size() > 0 ? utilities::type_cast<double>(axis[0]) : 0.0f,
                    axis.size() > 1 ? utilities::type_cast<double>(axis[1]) : 0.0f,
                    axis.size() > 2 ? utilities::type_cast<double>(axis[2]) : 0.0f,
                    axis.size() > 3 ? utilities::type_cast<double>(axis[3]) : 0.0f
                };
                params.push_back(vector);
            }
            // Numeric parameters can be integer or doubles
            else if (utilities::numeric(token)) {
                if (token.find(".") != std::string::npos) {
                    params.push_back(utilities::type_cast<double>(token));
                }
                else {
                    params.push_back(utilities::type_cast<int>(token));
                }
            }
            // Just add the token as-is
            else {
                params.push_back(token);
            }
        }
        
        // Call the command
        return commands[command].second(params);
    }

private:
    std::map<std::string, std::pair<std::string, callback_t>> commands = {
        { "/set",     { "/set [label] [value]\nset [entity] [label] [value]", &set } },
        { "/load",    { "/load [type] [name]", &load } },
        { "/create",  { "/create [type]", &create } },
        { "/show",    { "/show [type]", &show } },
        { "/exit",    { "/exit", &exit } },
    };
    std::map<int, callback_t> keybinds;

    static value_t set(parameters_t p) {
        if (p.size() < 2) {
            text_event(instance().commands["set"].first);
            return 0;
        }
        if (p.size() == 2) {
            scenes::global().set(std::get<label_t>(p[0]), p[1]);
        }
        if (p.size() == 3) {
            scenes::current().set(std::get<label_t>(p[0]), std::get<label_t>(p[1]), p[2]);
        }
        return p[1];
    }

    static value_t load(parameters_t p) {
        if (p.size() == 2) {
            auto type = std::get<std::string>(p[0]);
            auto name = std::get<std::string>(p[1]);
            scenes::global().load(type, name);
        }
        return 0;
    }

    static value_t create(parameters_t p) {
        return 0;
    }

    static value_t show(parameters_t p) {
        if (p.size() == 0) {
            text_event("show [commands|variables|assets|loaded]");
            return 0;
        }

        auto type = std::get<std::string>(p[0]);
        if (type == "loaded") {
            for (auto entry : scenes::global().fonts) {
                std::stringstream ss;
                ss << "font(" << entry.first << ")";
                text_event(ss.str());
            }
            for (auto entry : scenes::global().sounds) {
                std::stringstream ss;
                ss << "audio(" << entry.first << ")";
                text_event(ss.str());
            }
            for (auto entry : scenes::global().shaders) {
                std::stringstream ss;
                ss << "shader(" << entry.first << ")";
                text_event(ss.str());
            }
            for (auto entry : scenes::global().objects) {
                std::stringstream ss;
                ss << "object(" << entry.first << ")";
                text_event(ss.str());
            }
        }
        if (type == "variables") {
            for (auto variable : scenes::global().variables) {
                std::stringstream ss;
                if (std::holds_alternative<int>(variable.second)) {
                    ss << variable.first << ": " << std::get<int>(variable.second);
                }
                if (std::holds_alternative<double>(variable.second)) {
                    ss << variable.first << ": " << std::get<double>(variable.second);
                }
                if (std::holds_alternative<std::string>(variable.second)) {
                    ss << variable.first << ": " << std::get<std::string>(variable.second);
                }
                if (std::holds_alternative<spatial::vector>(variable.second)) {
                    auto v = std::get<spatial::vector>(variable.second);
                    ss << variable.first << ": (" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
                }
                text_event(ss.str());
            }
        }
        if (type == "commands") {
            for (auto command : actions::instance().commands) {
                text_event(command.second.first);
            }
        }
        if (type == "assets") {
            if (p.size() == 2) {
                auto path = std::get<std::string>(p[1]);
                for (auto asset : assets->list(path)) {
                    text_event(asset);
                }
            }
            else {
                text_event("show assets [path]");
            }
        }

        return 0;
    }

    static value_t exit(parameters_t p) {
        text_event("goodbye");
        exit;
        return 0; // exit doesn't actually exit in debug mode
    }
};

//=====================================================================================================

class splash : public scenes::scene {
public:
    void load() {
        actions::instance().call("load shader shader_basic");
    }

    void run() {
        utilities::sleep(10000);
    }
};

class title : public scenes::scene {
public:
    void load() {
        actions::instance().call("load sound glados");
        actions::instance().call("load font consolas-22");
    }

    void run() {

    }
};

class game : public scenes::scene {
public:
    void load() {
        
    }

    void run() {

    }

    float prior_x;
    float prior_y;

    spatial::vector mouse;

    void freelook_start(const platform::input::event& ev) {
        prior_x = ev.point.x;
        prior_y = ev.point.y;
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_start(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
    }

    void freelook_move(const platform::input::event& ev) {
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_move(" << ev.identifier << ")(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
        camera.pitch(ev.point.y - prior_y);
        camera.spin(prior_x - ev.point.x);
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    void freelook_zoom(const platform::input::event& ev) {
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "on_zoom(" << ev.travel << ")(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
        camera.move(ev.travel > 0 ? 0.1 : -0.1);
    }

    void mouse_move(const platform::input::event& ev) {
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
        mouse = ev.point;
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    bool camera_moving[4] = { false, false, false, false };
    bool object_moving[6] = { false, false, false, false, false, false };
    void keyboard_input(const platform::input::event& ev) {
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "key(" << ev.identifier << ")";
            text_event(ss.str());
        }
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
                //projectiles.push_back(pos);
                //if (projectiles.size() > 10) {
                //    projectiles.pop_front();
                //}
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
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            if (ev.gesture == platform::input::DOWN || ev.gesture == platform::input::HELD) {
                ss << "button_down(" << ev.identifier << ")";
            }
            if (ev.gesture == platform::input::UP) {
                ss << "button_up(" << ev.identifier << ")";
            }
            text_event(ss.str());
        }
    }
};

class debug : public scenes::scene {
public:
    void load() {

    }

    void run() {

    }

    void stop() {

    }
};


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

inline float prior_x;
inline float prior_y;

void freelook_start(const platform::input::event& ev) {
    prior_x = ev.point.x;
    prior_y = ev.point.y;
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        ss << "freelook_start(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
    }
}

void freelook_move(const platform::input::event& ev) {
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        ss << "freelook_move(" << ev.identifier << ")(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
    }
    camera.pitch(ev.point.y - prior_y);
    camera.spin(prior_x - ev.point.x);
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

void freelook_zoom(const platform::input::event& ev) {
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        ss << "on_zoom(" << ev.travel << ")(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
    }
    camera.move(ev.travel > 0 ? 0.1 : -0.1);
}

void mouse_move(const platform::input::event& ev) {
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
        text_event(ss.str());
    }
    mouse = ev.point;
    prior_x = ev.point.x;
    prior_y = ev.point.y;
}

bool camera_moving[4] = { false, false, false, false };
bool object_moving[6] = { false, false, false, false, false, false };
void keyboard_input(const platform::input::event& ev) {
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        ss << "key(" << ev.identifier << ")";
        text_event(ss.str());
    }
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
    if (scenes::global().flag("debug.input")) {
        std::stringstream ss;
        if (ev.gesture == platform::input::DOWN || ev.gesture == platform::input::HELD) {
            ss << "button_down(" << ev.identifier << ")";
        }
        if (ev.gesture == platform::input::UP) {
            ss << "button_up(" << ev.identifier << ")";
        }
        text_event(ss.str());
    }
}

void prototype::on_startup() {
    graphics->init();
    audio->init();

    bounds = spatial::quad(1, 1);

    actions::instance().call("load shader shader_basic");
    actions::instance().call("load audio glados");

    //assets->retrieve("objects/wiggle.fbx") >> format::parser::fbx >> wiggle;
    //graphics->compile(wiggle.children[0]);

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

    assets->retrieve("objects/ground/ground.obj") >> format::parser::obj >> ground;
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
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
        gui->print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap, don't intend to ever do this anyway
    }, 1)).handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "button_down(" << ev.point.x << ", " << ev.point.y << ")";
            text_event(ss.str());
        }
        //audio->start(sound);
        client->connect();
        actions::instance().call("show assets objects");
        actions::instance().call("show assets objects/ground");
        }, 1);

    events = gui->create(platform::interface::widget::spec::textbox, 512, 720, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 20).id;
    gui->get<platform::interface::textbox>(events).alignment = platform::interface::widget::positioning::bottom;

    textbox = gui->create(platform::interface::widget::spec::textbox, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 750).id;
    gui->get<platform::interface::textbox>(textbox).selectable = true;
    gui->get<platform::interface::textbox>(textbox).handler(platform::input::KEY, platform::input::DOWN, [](const platform::input::event& ev) {
        //std::stringstream ss;
        std::vector<std::string> content;
        switch (ev.identifier) {
        case(8): // Backspace to remove a character
            gui->get<platform::interface::textbox>(textbox).content.remove(1);
            break;
        case(13): // Enter to submit
            content = gui->get<platform::interface::textbox>(textbox).content.get();
            if (content.size()) {
                //ss << "text_submit(" << content[0] << ")";
                //text_event(ss.str());
                gui->get<platform::interface::textbox>(textbox).content.remove(-1);
                actions::instance().call(content[0]);
            }
            break;
        default: // Every other printable gets added to the contents
            gui->get<platform::interface::textbox>(textbox).content.append(input->printable(ev.identifier));
        };
    });

    progress = gui->create(platform::interface::widget::spec::progress, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 780).id;

    server->handler([](platform::network::client* caller) {
        std::string input(caller->input.begin(), caller->input.end());
        if (scenes::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "client_message(" << input << ")";
            text_event(ss.str());
        }
        audio->start(sound);
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
