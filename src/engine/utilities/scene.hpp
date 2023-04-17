/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

namespace stage {

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

        virtual void show(label_t label) {}

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

    platform::interface::textbox debug;
    platform::interface::progress progress;
    
    grid map;

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
        { "/test",    { "/test <command> [group]", [](parameters_t p)->value_t { return scene::global()._test(p); } } },
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
    value_t _test(parameters_t p);
};

}
