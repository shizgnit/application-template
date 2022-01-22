#include "engine.hpp"

/*
TODO
time based animation and object manipulation
object automated motion and movement/camera constraints
input abstraction for user specified keybinds
network communication updated for HTTPS
cell shader
particle system
shadows
*/

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

    std::list<spatial::position> path;
    spatial::position position;
};

/// <summary>
/// Organizes the resources, interface and actions that are currently active
/// as well as transations between them.
/// - Title
/// - Configuration
/// - Results
/// </summary>
class main {
public:
    class scene {
    public:
        scene() {}
        virtual ~scene() {}

        std::map<label_t, value_t> variables;
        std::map<int, entity> entities;

        virtual bool load() { return true; }
        virtual void start() {}
        virtual void run() {}
        virtual void stop() {}

        virtual void freelook_start(const platform::input::event& ev) {}
        virtual void freelook_move(const platform::input::event& ev) {}
        virtual void freelook_zoom(const platform::input::event& ev) {}
        virtual void mouse_move(const platform::input::event& ev) {}
        virtual void keyboard_input(const platform::input::event& ev) {}
        virtual void gamepad_input(const platform::input::event& ev) {}

        void set(std::string entity, label_t label, value_t value) {

        }

        void geometry(int width, int height) {

        }

        bool loaded = false;
        bool displayed = false;

        spatial::position camera;
    };

    static platform::interface::textbox& debug() {
        static platform::interface::textbox instance;
        return instance;
    }

    static platform::interface::progress& progress() {
        static platform::interface::progress instance;
        return instance;
    }

    static main& global() {
        static main singleton;
        return singleton;
    }

    /// <summary>
    /// Scene management methods
    /// </summary>

    void add(std::string name, scene* instance) {
        std::lock_guard<std::mutex> scoped(lock);
        scenes[name] = instance;
    }

    bool toggle(std::string name) {
        bool isactive = false;
        {
            std::lock_guard<std::mutex> scoped(lock);
            if (name.empty() || scenes.find(name) == scenes.end()) {
                return false;
            }
            isactive = (active.find(name) != active.end());
        }
        if (isactive) {
            return deactivate(name);
        }
        else {
            return activate(name);
        }
    }

    bool activate(std::string name) {
        std::lock_guard<std::mutex> scoped(lock);
        if (name.empty() || scenes.find(name) == scenes.end()) {
            return false;
        }
        while (scenes[name]->loaded == false && scenes[name]->load() == false) {
            scenes[name]->loaded = true;
        }
        scenes[name]->start();
        active[name] = scenes[name];
        return true;
    }

    bool deactivate(std::string name) {
        std::lock_guard<std::mutex> scoped(lock);
        if (name.empty() || active.find(name) == active.end()) {
            return false;
        }
        auto reference = active[name];
        active.erase(name);
        reference->stop();
        return true;
    }

    bool transition(std::string from, std::string to) {
        transitions.clear();
        if (to.empty() || scenes.find(to) == scenes.end()) {
            return false;
        }
        if (scenes[to]->loaded == false && scenes[to]->load() == false) {
            transitions.push_back({ from, to });
            return false;
        }
        scenes[to]->loaded = true;
        deactivate(from);
        activate(to);
        return true;
    }

    void run() {
        if (transitions.size()) {
            transition(transitions.front().first, transitions.front().second);
        }
        auto list = active;
        for (auto scene : list) {
            scene.second->run();
        }
    }

    std::map<std::string, scene*> current() {
        return active;
    }

    /// <summary>
    /// Event propagation to the active scenes
    /// </summary>

    void freelook_start(const platform::input::event& ev) {
        for (auto scene : active) {
           scene.second->freelook_start(ev);
        }        
    }
    void freelook_move(const platform::input::event& ev) {
        for (auto scene : active) {
            scene.second->freelook_move(ev);
        }
    }
    void freelook_zoom(const platform::input::event& ev) {
        for (auto scene : active) {
            scene.second->freelook_zoom(ev);
        }
    }
    void mouse_move(const platform::input::event& ev) {
        for (auto scene : active) {
            scene.second->mouse_move(ev);
        }
    }
    void keyboard_input(const platform::input::event& ev) {
        if (ev.gesture == platform::input::UP && ev.identifier == 192) {
            toggle("debug");
        }
        for (auto scene : current()) {
            scene.second->keyboard_input(ev);
        }
    }
    void gamepad_input(const platform::input::event& ev) {
        for (auto scene : active) {
            scene.second->gamepad_input(ev);
        }
    }

    void geometry(int width, int height) {
        int fov = has("perspective.fov") ? std::get<int>(main::global().get("perspective.fov")) : 90;

        ortho.ortho(0, width, 0, height);
        perspective.perspective(fov * (float)M_PI / 180.0f, (float)width / (float)height, -1.0f, 1.0f);

        std::lock_guard<std::mutex> scoped(lock);
        for (auto scene : current()) {
            scene.second->geometry(width, height);
        }
    }

    spatial::matrix ortho;
    spatial::matrix perspective;

    std::map<std::string, type::font> fonts;
    std::map<std::string, type::program> shaders;
    std::map<std::string, type::audio> sounds;
    std::map<std::string, type::object> objects;

    std::map<label_t, value_t> variables;

    std::map<std::string, scene*> scenes;
    std::map<std::string, scene*> active;

    std::mutex lock;

    std::list<std::pair<std::string, std::string>> transitions;

  public:
    typedef std::vector<value_t> parameters_t;
    typedef std::function<value_t(parameters_t)> callback_t;

    value_t call(const std::string& input) {
        auto tokens = utilities::tokenize(input, " ");
        if (tokens.empty() || commands.find(tokens[0]) == commands.end()) {
            std::vector<std::string> list;
            for (auto command : commands) {
                list.push_back(command.first);
            }
            std::stringstream ss;
            ss << "commands: " << utilities::join(", ", list);
            main::debug().content.add(ss.str());
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
                    axis.size() > 3 ? utilities::type_cast<double>(axis[3]) : 1.0f
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
        value_t result = commands[command].second(params);

        // Debug the call and result
        std::stringstream ss;
        ss << input << " = ";
        if (std::holds_alternative<int>(result)) {
            ss << std::get<int>(result);
        }
        if (std::holds_alternative<double>(result)) {
            ss << std::get<double>(result);
        }
        if (std::holds_alternative<std::string>(result)) {
            ss << std::get<std::string>(result);
        }
        if (std::holds_alternative<spatial::vector>(result)) {
            auto& v = std::get<spatial::vector>(result);
            ss << "(" << v.x << "," << v.x << "," << v.x << "," << v.x << ")";
        }
        main::debug().content.add(ss.str());

        return result;
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

    bool has(label_t label) {
        return variables.find(label) != variables.end();
    }

    value_t get(label_t label) {
        return variables[label];
    }

    value_t set(label_t label, value_t value) {
        variables[label] = value;
        if (label == "perspective.fov") {
            geometry(graphics->width(), graphics->height());
        }

        return value;
    }

private:
    std::map<std::string, std::pair<std::string, callback_t>> commands = {
        { "/get",     { "/get <label>\n/get <entity> <label>", [](parameters_t p)->value_t { return main::global().get(p); } } },
        { "/set",     { "/set <label> <value>\n/set <entity> <label> <value>", [] (parameters_t p)->value_t { return main::global().set(p); } } },
        { "/load",    { "/load <type> <source> [target]", [](parameters_t p)->value_t { return main::global().load(p); } } },
        { "/create",  { "/create <type>", [](parameters_t p)->value_t { return main::global().create(p); } } },
        { "/show",    { "/show <type>", [](parameters_t p)->value_t { return main::global().show(p); } } },
        { "/exit",    { "/exit", [](parameters_t p)->value_t { return main::global().exit(p); } } },
    };
    std::map<int, callback_t> keybinds;

    value_t get(parameters_t p) {
        if (p.size() < 1) {
            main::debug().content.add(commands["get"].first);
            return 0;
        }
        return get(std::get<label_t>(p[0]));
    }

    value_t set(parameters_t p) {
        if (p.size() < 2) {
            main::debug().content.add(commands["set"].first);
            return 0;
        }
        if (p.size() == 2) {
            set(std::get<label_t>(p[0]), p[1]);
        }
        if (p.size() == 3) {
            std::lock_guard<std::mutex> scoped(lock);
            for (auto scene : active) {
                scene.second->set(std::get<label_t>(p[0]), std::get<label_t>(p[1]), p[2]);
            }
        }
        return p[1];
    }

    value_t load(parameters_t p) {
        if (p.size() >= 2) {
            auto type = std::get<std::string>(p[0]);
            auto source = std::get<std::string>(p[1]);
            auto target = p.size() == 3 ? std::get<std::string>(p[2]) : source;

            auto tokens = utilities::tokenize(source, "/");

            std::string resource = tokens.size() == 2 ? tokens[1] : tokens[0];

            if (type == "audio") {
                std::string path = tokens.size() == 2 ? tokens[0] : "raw";
                assets->retrieve(path + "/" + resource + ".wav") >> format::parser::wav >> sounds[target];
                audio->compile(sounds[target]);
            }
            if (type == "shader") {
                std::string path = tokens.size() == 2 ? tokens[0] : "shaders";
                assets->retrieve(path + "/" + resource + ".vert") >> format::parser::vert >> shaders[target].vertex;
                assets->retrieve(path + "/" + resource + ".frag") >> format::parser::frag >> shaders[target].fragment;
                shaders[target].compiled(false);
                if (graphics->compile(shaders[target]) == false) {
                    while (graphics->messages()) {
                        main::debug().content.add(graphics->message()); 
                    }
                }
            }
            if (type == "font") {
                std::string path = tokens.size() == 2 ? tokens[0] : "fonts";
                assets->retrieve(path + "/" + resource + ".fnt") >> format::parser::fnt >> fonts[target];
                graphics->compile(fonts[target]);
            }
            if (type == "object") {
                std::string path = tokens.size() == 2 ? tokens[0] : "objects";
                if (resource.substr(resource.size() - 4, 4) == ".fbx") {
                    target = target == resource ? resource.substr(0, resource.size() - 4) : target;
                    assets->retrieve(path + "/" + resource) >> format::parser::fbx >> objects[target];
                }
                else if (resource.substr(resource.size() - 4, 4) == ".obj") {
                    target = target == resource ? resource.substr(0, resource.size() - 4) : target;
                    assets->retrieve(path + "/" + resource) >> format::parser::obj >> objects[target];
                }
                else if (resource.substr(resource.size() - 4, 4) == ".png") {
                    target = target == resource ? resource.substr(0, resource.size() - 4) : target;
                    objects[target] = spatial::quad(256, 256);
                    assets->retrieve(path + "/" + resource) >> format::parser::png >> objects[target].texture.map;
                    objects[target].xy_projection(0, 0, objects[target].texture.map.properties.width, objects[target].texture.map.properties.height);
                }
                else {
                    assets->retrieve(path + "/" + resource + ".obj") >> format::parser::obj >> objects[target];
                }
                graphics->compile(objects[target]);
            }
            if (type == "entity") {
                // TODO
            }
        }
        return 0;
    }

    value_t create(parameters_t p) {
        return 0;
    }

    value_t show(parameters_t p) {
        if (p.size() == 0) {
            main::debug().content.add("/show [commands|variables|assets|loaded]");
            return 0;
        }

        auto type = std::get<std::string>(p[0]);
        if (type == "loaded") {
            for (auto entry : fonts) {
                std::stringstream ss;
                ss << "font(" << entry.first << ")";
                main::debug().content.add(ss.str());
            }
            for (auto entry : sounds) {
                std::stringstream ss;
                ss << "audio(" << entry.first << ")";
                main::debug().content.add(ss.str());
            }
            for (auto entry : shaders) {
                std::stringstream ss;
                ss << "shader(" << entry.first << ")";
                main::debug().content.add(ss.str());
            }
            for (auto entry : objects) {
                std::stringstream ss;
                ss << "object(" << entry.first << ")";
                main::debug().content.add(ss.str());
            }
        }
        if (type == "variables") {
            for (auto variable : variables) {
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
                main::debug().content.add(ss.str());
            }
        }
        if (type == "commands") {
            for (auto command : commands) {
                main::debug().content.add(command.second.first);
            }
        }
        if (type == "assets") {
            if (p.size() == 2) {
                auto path = std::get<std::string>(p[1]);
                for (auto asset : assets->list(path)) {
                    main::debug().content.add(asset);
                }
            }
            else {
                main::debug().content.add("/show assets [path]");
            }
        }

        return 0;
    }

    static value_t exit(parameters_t p) {
        main::debug().content.add("goodbye");
        exit;
        return 0; // exit doesn't actually exit in debug mode
    }
};

//=====================================================================================================

class console : public main::scene {
public:
    bool load() {
        gui->create(&main::debug(), 512, 720, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 20);
        main::debug().selectable = true;
        main::debug().alignment = platform::interface::widget::positioning::bottom;

        gui->create(&commandline, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 750);
        commandline.selectable = true;
        commandline.input = true;
        commandline.multiline = false;
        commandline.content.limit = 1; // TODO: This should be covered by the multiline flag, but currently isn't.
        commandline.handler(platform::input::KEY, platform::input::DOWN, [this](const platform::input::event& ev) {
            std::vector<std::string> content;
            switch (ev.identifier) {
            case(38):
                commandline.content.position(1);
                break;
            case(40):
                commandline.content.position(-1);
                break;
            case(8): // Backspace to remove a character
                commandline.content.truncate(1);
                break;
            case(13): // Enter to submit
                content = commandline.content.get();
                if (content.size() && content.back().size() && content.back()[0] == '/') {
                    main::global().call(content.back());
                }
                else {
                    main::debug().content.add(content.back());
                }
                if (commandline.content.position() == 0) {
                    commandline.content.add();
                }
                commandline.content.position(commandline.content.size() * -1);
                break;
            default: // Every other printable gets added to the contents
                commandline.content.append(input->printable(ev.identifier));
            };
        });

        // Start off with an empty command
        commandline.content.add();

        return true;
    }

    void start() {
        main::debug().visible = true;
        gui->select(&commandline);
        commandline.visible = true;
        gui->position();
    }

    void stop() {
        gui->select(NULL);
        main::debug().visible = false;
        commandline.visible = false;
    }

    platform::interface::textbox commandline;
};

class splash : public main::scene {
public:
    void run() {
        if (start == 0) {
            start = time(NULL);
        }
        if (time(NULL) - start > 1) {
            main::global().transition("splash", "title");
        }
    }

    time_t start = 0;
};

class title : public main::scene {
public:
    bool load() {
        main::global().call("/set debug.input 0");

        main::global().call("/set ambient.position (5,5,5)");
        main::global().call("/set ambient.lookat (0,0,0)");
        main::global().call("/set ambient.color (0.4,0.4,0.4)");

        main::global().call("/set box1.position (0,-8,0)");
        main::global().call("/set box2.position (5,-7.7,0)");

        main::global().call("/set perspective.fov 90");

        main::global().call("/set shadow.scale 42");

        main::global().call("/load sound glados");
        main::global().call("/load shader basic basic");
        main::global().call("/load shader cell cell");
        main::global().call("/load shader defuse defuse");
        main::global().call("/load shader shadowmap shadowmap");
        main::global().call("/load shader depth_to_color depth");
        main::global().call("/load shader basic gui");
        main::global().call("/load shader basic skybox");
        main::global().call("/load shader cell objects");
        main::global().call("/load shader defuse_with_shadows scenery");
        main::global().call("/load font consolas-22");

        main::global().call("/load object drawable/marvin.png icon");

        gui->shader = main::global().shaders["gui"];
        gui->font = main::global().fonts["consolas-22"];

        gui->create(&main::global().progress(), graphics->width() / 2, 20, 0, 0, 0, 80).position(graphics->width() / 2 / 2, graphics->height() - 80);

        gui->create(&enter, 256, 256, 0, 0, 0, 80).position(20, 20).handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
            if (main::global().flag("debug.input")) {
                std::stringstream ss;
                ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
                main::debug().content.add(ss.str());
            }
            gui->print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap, don't intend to ever do this anyway
        }, 1).handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
            if (main::global().flag("debug.input")) {
                std::stringstream ss;
                ss << "button_down(" << ev.point.x << ", " << ev.point.y << ")";
                main::debug().content.add(ss.str());
            }
            client->connect();
            //audio->start(main::global().sounds["glados"]);
            main::global().progress().visible = true;
            main::global().transition("title", "game");
        }, 1);

        main::global().toggle("debug"); // no current way to turn this on in android

        return true;
    }

    void start() {
        enter.visible = true;
        enter.enabled = true;

        main::global().progress().visible = false;

        gui->position();

    }
    void run() {
        spatial::matrix frame;
        frame.identity();
        frame.translate(20, graphics->height() - 20 - 256, 0);

        graphics->draw(main::global().objects["icon"], main::global().shaders["gui"], frame, spatial::matrix(), main::global().ortho);
    }
    void stop() {
        enter.visible = false;
        enter.enabled = false;

        main::global().progress().visible = false;
    }

    platform::interface::button enter;
};

class game : public main::scene {
public:
    type::object xAxis;
    type::object yAxis;
    type::object zAxis;

    type::object skybox;
    type::object box;
    type::object ground;

    type::object monkey;

    type::object ray;

    bool load() {
        // TODO: don't use the progress bar to determine thread completion
        if (main::global().progress().value.get() == 0) {
            main::global().progress().value.set(1);
            std::thread([this]{
                xAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(2.0, 0.0, 0.0));
                xAxis.texture.map.create(1, 1, 255, 0, 0, 255);
                xAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(10);

                yAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 2.0, 0.0));
                yAxis.texture.map.create(1, 1, 0, 255, 0, 255);
                yAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(20);

                zAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 0.0, 2.0));
                zAxis.texture.map.create(1, 1, 0, 0, 255, 255);
                zAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(30);
                assets->retrieve("objects/skybox.obj") >> format::parser::obj >> skybox;

                main::global().progress().value.set(40);
                assets->retrieve("objects/untitled.obj") >> format::parser::obj >> box;

                main::global().progress().value.set(50);
                assets->retrieve("objects/ground/ground.obj") >> format::parser::obj >> ground;

                main::global().progress().value.set(60);
                assets->retrieve("objects/monkey.obj") >> format::parser::obj >> monkey;

                // Just to simulate when more resources are required
                utilities::sleep(1000);
                main::global().progress().value.set(70);
                utilities::sleep(1000);
                main::global().progress().value.set(80);
                utilities::sleep(1000);
                main::global().progress().value.set(90);
                utilities::sleep(1000);
                main::global().progress().value.set(100);
             }).detach();
        }
        if (main::global().progress().value.get() < 100) {
            return false;
        }

        graphics->compile(xAxis);
        graphics->compile(yAxis);
        graphics->compile(zAxis);
        graphics->compile(skybox);
        graphics->compile(box);
        graphics->compile(ground);
        graphics->compile(monkey);

        ray = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
        ray.texture.map.create(1, 1, 255, 255, 255, 255);
        ray.xy_projection(0, 0, 1, 1);
        graphics->compile(ray);


        /*
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
        */

        return true;
    }

    void draw(spatial::position pos, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options = 0x00) {
        
        ray = spatial::ray(pos.center, pos.eye);
        ray.texture.map.create(1, 1, 255, 0, 0, 255);
        graphics->recompile(ray);
        graphics->draw(ray, shader, model, view, projection, spatial::matrix(), options);

        ray = spatial::ray(pos.center, pos.up + pos.center);
        ray.texture.map.create(1, 1, 0, 255, 0, 255);
        graphics->recompile(ray);
        graphics->draw(ray, shader, model, view, projection, spatial::matrix(), options);

    }

    void run() {
        auto& shader_basic = main::global().shaders["basic"];
        auto& shader_shadowmap = main::global().shaders["shadowmap"];
        auto& shader_defuse = main::global().shaders["defuse"];
        auto& shader_skybox = main::global().shaders["skybox"];
        auto& shader_objects = main::global().shaders["objects"];
        auto& shader_scenery = main::global().shaders["scenery"];

        auto& perspective = main::global().perspective;

        auto position = std::get<spatial::vector>(main::global().get("ambient.position"));
        auto lookat = std::get<spatial::vector>(main::global().get("ambient.lookat"));

        graphics->ambient.position.reposition(position);
        graphics->ambient.position.lookat(lookat);

        graphics->ambient.color = std::get<spatial::vector>(main::global().get("ambient.color"));

        // View based on the camera
        spatial::matrix lighting = spatial::matrix().lookat(graphics->ambient.position.eye, graphics->ambient.position.center, graphics->ambient.position.up);
        spatial::matrix view = spatial::matrix().lookat(camera.eye, camera.center, camera.up);

        // orthographic view matrix relative to the target
        spatial::matrix ortho;
        auto scale = std::get<int>(main::global().get("shadow.scale"));
        ortho.ortho(scale, scale * -1.0f, scale, scale * -1.0f);

        auto box1_matrix = spatial::matrix().translate(std::get<spatial::vector>(main::global().get("box1.position")));
        auto box2_matrix = spatial::matrix().translate(std::get<spatial::vector>(main::global().get("box2.position"))).scale(2.0f);

        {
            auto scoped = graphics->target(graphics->shadow);

            graphics->draw(box, shader_shadowmap, box1_matrix, lighting, ortho, ortho * lighting);
            graphics->draw(box, shader_shadowmap, box2_matrix, lighting, ortho, ortho * lighting);
            graphics->draw(monkey, shader_shadowmap, spatial::matrix().translate(0, -2, -10).scale(5.0f), lighting, ortho, ortho * lighting);
        }

        graphics->draw(skybox, shader_skybox, spatial::matrix(), view, perspective);

        graphics->draw(ground, shader_scenery, spatial::matrix().scale(4.0f), view, perspective, ortho * lighting, platform::graphics::render::NORMALS);

        graphics->draw(box, shader_defuse, box1_matrix, view, perspective, ortho * lighting, platform::graphics::render::NORMALS);
        graphics->draw(box, shader_defuse, box2_matrix, view, perspective, ortho * lighting, platform::graphics::render::NORMALS);

        graphics->draw(xAxis, shader_basic, spatial::matrix(), view, perspective);
        graphics->draw(yAxis, shader_basic, spatial::matrix(), view, perspective);
        graphics->draw(zAxis, shader_basic, spatial::matrix(), view, perspective);

        graphics->draw(monkey, shader_objects, spatial::matrix().translate(0, -2, -10).scale(5.0f), view, perspective);

        draw(graphics->ambient.position, shader_basic, spatial::matrix(), view, perspective);

        spatial::matrix frame;
        frame.identity();
        frame.translate(20, graphics->height() - 20 - 256, 0);

        graphics->draw(graphics->shadow, graphics->shadow.texture.depth ? main::global().shaders["depth"] : shader_basic, frame, spatial::matrix(), main::global().ortho);

        /*
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
        */

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

        //spatial::matrix model = spatial::matrix().translate(pos.eye, pos.center, pos.up);
        {
            // This entire scope will render to the poly texture, every frame... which is unnecessary, just testing for performance, etc.
            auto scoped = graphics->target(box);

            // just moving it a bit to move away from the edges
            spatial::matrix rendertotex;
            rendertotex.identity();
            rendertotex.translate(20, 20, 0);

            // orthographic view matrix relative to the target
            spatial::matrix ortho;
            ortho.ortho(0, box.texture.map.properties.width, 0, box.texture.map.properties.height);

            graphics->draw(main::global().objects["icon"], shader_basic, rendertotex, spatial::matrix(), ortho);
        }

        /*
        for (auto &projectile : projectiles) {
            spatial::matrix position = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
            trail = position.interpolate(spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2)));
            graphics->recompile(trail);

            graphics->draw(trail, shader, spatial::matrix(), view, perspective);

            projectile.move(0.4);

            spatial::matrix model = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
            graphics->draw(ray, shader, model, view, perspective);
        }
        */
    }

    float prior_x;
    float prior_y;

    spatial::vector mouse;

    void freelook_start(const platform::input::event& ev) {
        prior_x = ev.point.x;
        prior_y = ev.point.y;
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_start(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
    }

    void freelook_move(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_move(" << ev.identifier << ")(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        camera.pitch(ev.point.y - prior_y);
        camera.spin(prior_x - ev.point.x);
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    void freelook_zoom(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "on_zoom(" << ev.travel << ")(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        camera.move(ev.travel > 0 ? 1.0 : -1.0);
    }

    void mouse_move(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        mouse = ev.point;
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    bool camera_moving[4] = { false, false, false, false };
    bool object_moving[6] = { false, false, false, false, false, false };
    void keyboard_input(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "key(" << ev.identifier << ")";
            main::debug().content.add(ss.str());
        }

        if (ev.gesture == platform::input::DOWN && gui->active() == false) {
            switch (ev.identifier) {
            case(27):
                main::global().transition("game", "title");
                break;
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
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            if (ev.gesture == platform::input::DOWN || ev.gesture == platform::input::HELD) {
                ss << "button_down(" << ev.identifier << ")";
            }
            if (ev.gesture == platform::input::UP) {
                ss << "button_up(" << ev.identifier << ")";
            }
            main::debug().content.add(ss.str());
        }
    }
};

//=====================================================================================================

void prototype::on_startup() {
    graphics->init();
    audio->init();

    /*
    server->handler([](platform::network::client* caller) {
        std::string input(caller->input.begin(), caller->input.end());
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "client_message(" << input << ")";
            main::debug().content.add(ss.str());
        }
        audio->start(sound);
    });
    */

    // Hook up the input handlers
    input->handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) { main::global().freelook_start(ev); }, 2);
    input->handler(platform::input::POINTER, platform::input::DRAG, [](const platform::input::event& ev) { main::global().freelook_move(ev); }, 0);

    input->handler(platform::input::POINTER, platform::input::WHEEL, [](const platform::input::event& ev) { main::global().freelook_zoom(ev); }, 0);
    input->handler(platform::input::POINTER, platform::input::PINCH, [](const platform::input::event& ev) { main::global().freelook_zoom(ev); }, 0);

    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) { main::global().mouse_move(ev); }, 0);

    input->handler(platform::input::KEY, platform::input::DOWN, [](const platform::input::event& ev) { main::global().keyboard_input(ev); }, 0);
    input->handler(platform::input::KEY, platform::input::UP, [](const platform::input::event& ev) { main::global().keyboard_input(ev); }, 0);

    input->handler(platform::input::GAMEPAD, platform::input::DOWN, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);
    input->handler(platform::input::GAMEPAD, platform::input::HELD, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);
    input->handler(platform::input::GAMEPAD, platform::input::UP, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);

    main::global().add("debug", new console());
    main::global().add("splash", new splash());
    main::global().add("title", new title());
    main::global().add("game", new game());

    main::global().activate("splash");

    gui->position();

    //server->start();
}

void prototype::on_resize() {
    graphics->geometry(width, height);

    main::global().geometry(width, height);

    gui->projection = main::global().ortho;

    gui->position();
}

void prototype::on_draw() {
    graphics->clear();

    main::global().run();

    gui->draw();
    graphics->flush();
}

void prototype::on_interval() {
    main::debug().content.add("on_proc");
}
