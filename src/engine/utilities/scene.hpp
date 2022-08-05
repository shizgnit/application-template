#pragma once

/// <summary>
/// Organizes the resources, interface and actions that are currently active
/// as well as transations between them.
/// - Title
/// - Configuration
/// - Results
/// </summary>
class scene : public properties {
public:
    class handler : public properties {
    public:
        handler() {}
        virtual ~handler() {}

        virtual bool load() { return true; }
        virtual bool save() { return false; }
        virtual void start() {}
        virtual void draw() {}
        virtual void run() {}
        virtual void stop() {}

        virtual void freelook_start(const platform::input::event& ev) {}
        virtual void freelook_move(const platform::input::event& ev) {}
        virtual void freelook_zoom(const platform::input::event& ev) {}
        virtual void mouse_click(const platform::input::event& ev) {}
        virtual void mouse_move(const platform::input::event& ev) {}
        virtual void mouse_drag(const platform::input::event& ev) {}
        virtual void mouse_release(const platform::input::event& ev) {}
        virtual void mouse_scroll(const platform::input::event& ev) {}
        virtual void keyboard_input(const platform::input::event& ev) {}
        virtual void gamepad_input(const platform::input::event& ev) {}

        virtual void set(label_t label, value_t value) {}
        virtual void set(label_t entity, label_t label, value_t value) {}

        virtual void group(label_t identifier, label_t command) {}
        virtual void group(label_t identifier, label_t label, value_t value) {}

        virtual void dimensions(int width, int height) {}

        bool loaded = false;
        bool started = false;
        bool displayed = false;

        //spatial::position camera;
        spatial::position freelook;
    };

    class persistence {
        std::string _file;

    public:
        persistence();
        bool write();
        bool read();
    };


    static platform::interface::textbox& debug() {
        static platform::interface::textbox instance;
        return instance;
    }

    static platform::interface::progress& progress() {
        static platform::interface::progress instance;
        return instance;
    }

    inline static scene& global() {
        static scene instance;
        return instance;
    }

    /// <summary>
    /// Scene management methods
    /// </summary>

    void add(std::string name, handler* instance);
    bool load(std::string name);
    bool toggle(std::string name);
    bool isactive(std::string name);
    bool activate(std::string name);
    bool deactivate(std::string name);
    bool transition(std::string from, std::string to);
    void run();
    void draw();

    std::map<std::string, handler*> current();

    /// <summary>
    /// Event propagation to the active scenes, note that these have no inherent meaing.
    /// </summary>

    void freelook_start(const platform::input::event& ev);
    void freelook_move(const platform::input::event& ev);
    void freelook_zoom(const platform::input::event& ev);

    void mouse_click(const platform::input::event& ev);
    void mouse_move(const platform::input::event& ev);
    void mouse_drag(const platform::input::event& ev);
    void mouse_release(const platform::input::event& ev);
    void mouse_scroll(const platform::input::event& ev);

    void keyboard_input(const platform::input::event& ev);
    void gamepad_input(const platform::input::event& ev);

    void dimensions(int width, int height);

    std::map<std::string, handler*> scenes;
    std::map<std::string, handler*> active;

    std::mutex lock;

    std::list<std::pair<std::string, std::string>> transitions;

public:
    typedef std::vector<value_t> parameters_t;
    typedef std::function<value_t(parameters_t)> callback_t;

    value_t call(const std::string& input);

    value_t set(label_t label, value_t value);

    void callback(std::string label, callback_t c) {
        events[label] = c;
    }

private:
    std::map<std::string, callback_t> events;

    std::map<std::string, std::pair<std::string, callback_t>> commands = {
        { "/group",   { "/group <id> <command>\n/group <id> <label> <value>", [](parameters_t p)->value_t { return scene::global()._group(p); } } },
        { "/get",     { "/get <label>\n/get <entity> <label>", [](parameters_t p)->value_t { return scene::global()._get(p); } } },
        { "/set",     { "/set <label> <value>\n/set <entity> <label> <value>", [](parameters_t p)->value_t { return scene::global()._set(p); } } },
        { "/load",    { "/load <type> <source> [target]", [](parameters_t p)->value_t { return scene::global()._load(p); } } },
        { "/compile", { "/compile", [](parameters_t p)->value_t { return scene::global()._compile(p); } } },
        { "/play",    { "/play <entity> <state>", [](parameters_t p)->value_t { return scene::global()._play(p); } } },
        { "/create",  { "/create <type>", [](parameters_t p)->value_t { return scene::global()._create(p); } } },
        { "/show",    { "/show <type>", [](parameters_t p)->value_t { return scene::global()._show(p); } } },
        { "/save",    { "/save", [](parameters_t p)->value_t { return scene::global()._save(p); } } },
        { "/exit",    { "/exit", [](parameters_t p)->value_t { return scene::global()._exit(p); } } },
    };
    std::map<int, callback_t> keybinds;

    value_t _group(parameters_t p);
    value_t _get(parameters_t p);
    value_t _set(parameters_t p);
    value_t _load(parameters_t p);
    value_t _compile(parameters_t p);
    value_t _play(parameters_t p);
    value_t _create(parameters_t p);
    value_t _show(parameters_t p);
    value_t _save(parameters_t p);
    value_t _exit(parameters_t p);
};
