#pragma once

typedef std::string label_t;
typedef std::variant<double, int, std::string, spatial::vector> value_t;

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

        virtual bool load() { return true; }
        virtual void start() {}
        virtual void run() {}
        virtual void stop() {}

        virtual void freelook_start(const platform::input::event& ev) {}
        virtual void freelook_move(const platform::input::event& ev) {}
        virtual void freelook_zoom(const platform::input::event& ev) {}
        virtual void mouse_click(const platform::input::event& ev) {}
        virtual void mouse_move(const platform::input::event& ev) {}
        virtual void keyboard_input(const platform::input::event& ev) {}
        virtual void gamepad_input(const platform::input::event& ev) {}

        void set(std::string entity, label_t label, value_t value) {

        }

        void dimensions(int width, int height) {

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

    void add(std::string name, scene* instance);
    bool toggle(std::string name);
    bool activate(std::string name);
    bool deactivate(std::string name);
    bool transition(std::string from, std::string to);
    void run();

    std::map<std::string, scene*> current();

    /// <summary>
    /// Event propagation to the active scenes
    /// </summary>

    void freelook_start(const platform::input::event& ev);
    void freelook_move(const platform::input::event& ev);
    void freelook_zoom(const platform::input::event& ev);
    void mouse_click(const platform::input::event& ev);
    void mouse_move(const platform::input::event& ev);
    void keyboard_input(const platform::input::event& ev);
    void gamepad_input(const platform::input::event& ev);

    void dimensions(int width, int height);

    std::map<label_t, value_t> variables;

    std::map<std::string, scene*> scenes;
    std::map<std::string, scene*> active;

    std::mutex lock;

    std::list<std::pair<std::string, std::string>> transitions;

public:
    typedef std::vector<value_t> parameters_t;
    typedef std::function<value_t(parameters_t)> callback_t;

    value_t call(const std::string& input);
    bool flag(label_t label);
    bool has(label_t label);
    value_t get(label_t label);
    value_t set(label_t label, value_t value);

    void callback(std::string label, callback_t c) {
        events[label] = c;
    }

private:
    std::map<std::string, callback_t> events;

    std::map<std::string, std::pair<std::string, callback_t>> commands = {
        { "/get",     { "/get <label>\n/get <entity> <label>", [](parameters_t p)->value_t { return main::global().get(p); } } },
        { "/set",     { "/set <label> <value>\n/set <entity> <label> <value>", [](parameters_t p)->value_t { return main::global().set(p); } } },
        { "/load",    { "/load <type> <source> [target]", [](parameters_t p)->value_t { return main::global().load(p); } } },
        { "/compile", { "/compile", [](parameters_t p)->value_t { return main::global().compile(p); } } },
        { "/play",    { "/play <entity> <state>", [](parameters_t p)->value_t { return main::global().play(p); } } },
        { "/create",  { "/create <type>", [](parameters_t p)->value_t { return main::global().create(p); } } },
        { "/show",    { "/show <type>", [](parameters_t p)->value_t { return main::global().show(p); } } },
        { "/exit",    { "/exit", [](parameters_t p)->value_t { return main::global().exit(p); } } },
    };
    std::map<int, callback_t> keybinds;

    value_t get(parameters_t p);
    value_t set(parameters_t p);
    value_t load(parameters_t p);
    value_t compile(parameters_t p);
    value_t play(parameters_t p);
    value_t create(parameters_t p);
    value_t show(parameters_t p);
    value_t exit(parameters_t p);
};
