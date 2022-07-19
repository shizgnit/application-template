#include "engine.hpp"

void main::add(std::string name, scene* instance) {
    std::lock_guard<std::mutex> scoped(lock);
    scenes[name] = instance;
}

bool main::toggle(std::string name) {
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

bool main::isactive(std::string name) {
    std::lock_guard<std::mutex> scoped(lock);
    if (name.empty() || scenes.find(name) == scenes.end()) {
        return false;
    }
    return (active.find(name) != active.end());
}

bool main::activate(std::string name) {
    std::lock_guard<std::mutex> scoped(lock);
    if (name.empty() || scenes.find(name) == scenes.end()) {
        return false;
    }
    while (scenes[name]->loaded == false && scenes[name]->load() == false) {
        scenes[name]->loaded = true;
    }
    active[name] = scenes[name];
    scenes[name]->start();
    return true;
}

bool main::deactivate(std::string name) {
    std::lock_guard<std::mutex> scoped(lock);
    if (name.empty() || active.find(name) == active.end()) {
        return false;
    }
    auto reference = active[name];
    active.erase(name);
    reference->stop();
    return true;
}

bool main::transition(std::string from, std::string to) {
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

void main::draw() {
    auto list = active;
    for (auto scene : list) {
        scene.second->draw();
    }
}

void main::run() {
    if (transitions.size()) {
        transition(transitions.front().first, transitions.front().second);
    }
    auto list = active;
    for (auto scene : list) {
        scene.second->run();
    }
}

std::map<std::string, main::scene*> main::current() {
    return active;
}

/// <summary>
/// Event propagation to the active scenes
/// </summary>

void main::freelook_start(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_start(ev);
    }
}
void main::freelook_move(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_move(ev);
    }
}
void main::freelook_zoom(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_zoom(ev);
    }
}
void main::mouse_click(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_click(ev);
    }
}
void main::mouse_move(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_move(ev);
    }
}
void main::mouse_drag(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_drag(ev);
    }
}
void main::mouse_release(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_release(ev);
    }
}
void main::mouse_scroll(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_scroll(ev);
    }
}
void main::keyboard_input(const platform::input::event& ev) {
    if (ev.gesture == platform::input::UP && ev.identifier == 192) {
        toggle("debug");
    }
    for (auto scene : current()) {
        scene.second->keyboard_input(ev);
    }
}
void main::gamepad_input(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->gamepad_input(ev);
    }
}

void main::dimensions(int width, int height) {
    std::lock_guard<std::mutex> scoped(lock);
    for (auto scene : current()) {
        scene.second->dimensions(width, height);
    }
}

value_t main::call(const std::string& input) {
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
        // Booleans
        else if (token == "true") {
            params.push_back(true);
        }
        else if (token == "false") {
            params.push_back(false);
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
        ss << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
    }
    main::debug().content.add(ss.str());

    return result;
}

value_t main::set(label_t label, value_t value) {
    properties::set(label, value);
    if (events.find(label) != events.end()) {
        events[label](parameters_t()); // TODO: actually pass in the value
    }
    return value;
}

value_t main::_group(parameters_t p) {
    if (p.size() < 2) {
        main::debug().content.add(commands["group"].first);
        return 0;
    }
    std::lock_guard<std::mutex> scoped(lock);

    auto group = std::get<label_t>(p[0]);
    if (p.size() == 2) {
        for (auto scene : active) {
            scene.second->group(group, std::get<label_t>(p[1]));
        }
    }
    if (p.size() == 3) {
        type::entity::catalog::singleton().getGroup(group).set(std::get<label_t>(p[1]), p[2]);
        for (auto scene : active) {
            scene.second->group(group, std::get<label_t>(p[1]), p[2]);
        }
    }
    return p[1];
}

value_t main::_get(parameters_t p) {
    if (p.size() < 1) {
        main::debug().content.add(commands["get"].first);
        return 0;
    }
    return properties::get(std::get<label_t>(p[0]));
}

value_t main::_set(parameters_t p) {
    if (p.size() < 2) {
        main::debug().content.add(commands["set"].first);
        return 0;
    }
    if (p.size() == 2) {
        set(std::get<label_t>(p[0]), p[1]);
        //std::lock_guard<std::mutex> scoped(lock);
        for (auto scene : active) {
            scene.second->set(std::get<label_t>(p[0]), p[1]);
        }
    }
    if (p.size() == 3) {
        //std::lock_guard<std::mutex> scoped(lock);
        for (auto scene : active) {
            scene.second->set(std::get<label_t>(p[0]), std::get<label_t>(p[1]), p[2]);
        }
    }
    return p[1];
}

value_t main::_load(parameters_t p) {
    if (p.size() == 2) {
        assets->load(std::get<std::string>(p[0]), std::get<std::string>(p[1]));
    }
    if (p.size() == 3) {
        assets->load(std::get<std::string>(p[0]), std::get<std::string>(p[1]), std::get<std::string>(p[2]));
    }
    while (assets->messages()) {
        main::debug().content.add(assets->message());
    }
    return 0;
}

value_t main::_compile(parameters_t p) {
    graphics->compile(assets);
    while (graphics->messages()) {
        main::debug().content.add(graphics->message());
    }
    return 0;
}

value_t main::_play(parameters_t p) {
    if (p.size() == 2) {
        auto name = std::get<label_t>(p[0]);
        auto animation = std::get<label_t>(p[1]);
        assets->get<type::entity>(name).play(animation);
        return 1;
    }
    return 0;
}

value_t main::_create(parameters_t p) {
    return 0;
}

value_t main::_show(parameters_t p) {
    if (p.size() == 0) {
        main::debug().content.add("/show [commands|variables|assets|loaded|entities]");
        return 0;
    }

    auto type = std::get<std::string>(p[0]);
    if (type == "objects") {
        for (auto entry : assets->get<type::object>()) {
            std::stringstream ss;
            ss << "objects(" << entry->id() << ")";
            main::debug().content.add(ss.str());
        }
    }
    if (type == "entities") {
        for (auto entry : assets->get<type::entity>()) {
            std::stringstream ss;
            ss << "entity(" << entry->id() << ")";
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

value_t main::_save(parameters_t p) {
    auto list = active;
    for (auto scene : list) {
        scene.second->save();
    }
    return 0;
}

value_t main::_exit(parameters_t p) {
    main::debug().content.add("goodbye");
    ::exit(0);
}
