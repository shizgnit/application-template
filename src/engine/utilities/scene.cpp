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

#include "engine.hpp"

#include "picojson/picojson.h"

bool writeProperties(properties& input, picojson::object& output) {
    int values = 0;
    for (auto label : input.keys()) {
        auto value = input.get(label);
        if (std::holds_alternative<std::string>(value)) {
            output[label] = picojson::value(std::get<std::string>(value));
        }
        if (std::holds_alternative<bool>(value)) {
            output[label] = picojson::value(std::get<bool>(value));
        }
        if (std::holds_alternative<double>(value)) {
            output[label] = picojson::value(std::get<double>(value));
        }
        if (std::holds_alternative<int>(value)) {
            output[label] = picojson::value((double)std::get<int>(value));
        }
        if (std::holds_alternative<spatial::vector>(value)) {
            auto position = std::get<spatial::vector>(value);
            
            picojson::object vector;
            vector["x"] = picojson::value(position.x);
            vector["y"] = picojson::value(position.y);
            vector["z"] = picojson::value(position.z);
            vector["w"] = picojson::value(position.w);
            
            output[label] = picojson::value(vector);
        }
        values += 1;
    }
    return values > 0;
}

double get(picojson::value& node, const std::string& label, double fallback = 0.0f) {
    double value = fallback;
    if (node.contains(label)) {
        if (node.get(label).is<double>()) {
            value = node.get(label).get<double>();
        }
    }
    return value;
}

bool parseProperties(picojson::value& input, properties& output) {
    if (input.is<picojson::object>() == false) {
        return false;
    }

    for (auto& attribute : input.get<picojson::object>()) {
        if (attribute.second.is<std::string>()) {
            output.set(attribute.first, attribute.second.get<std::string>());
        }
        if (attribute.second.is<bool>()) {
            output.set(attribute.first, attribute.second.get<bool>());
        }
        if (attribute.second.is<double>()) {
            output.set(attribute.first, attribute.second.get<double>());
        }
        if (attribute.second.is<picojson::object>()) {
            spatial::vector value;
            
            for(auto& property: attribute.second.get<picojson::object>()) {
                if(property.first == "x") {
                    value.x = property.second.get<double>();
                }
                if(property.first == "y") {
                    value.y = property.second.get<double>();
                }
                if(property.first == "z") {
                    value.z = property.second.get<double>();
                }
                if(property.first == "w") {
                    value.x = property.second.get<double>();
                }
            }
            
            output.set(attribute.first, value);
        }
    }

    return true;
}

bool parse(const std::string& data) {
    picojson::value input;
    std::string error = picojson::parse(input, data);
    if (error.empty() == false) {
        return false;
    }

    if (input.get("entities").is<picojson::object>() == false) {
        return false;
    }
    auto& entities = input.get("entities").get<picojson::object>();

    picojson::object groups;
    if (input.get("groups").is<picojson::object>()) {
        groups = input.get("groups").get<picojson::object>();
    }

    picojson::object blueprints;
    if (input.get("blueprints").is<picojson::object>()) {
        blueprints = input.get("blueprints").get<picojson::object>();
    }

    float increment = 99 / (float)(entities.size() + groups.size() + blueprints.size());
    float percentage = 0;

    for (auto& entity : entities) {
        auto& reference = assets->get<type::entity>(entity.first);
        if (entity.second.contains("properties")) {
            parseProperties(entity.second.get("properties"), reference);
        }
        stage::scene::global().call("/load entity " + entity.first);
        
        if (entity.second.contains("instances") == false || entity.second.get("instances").get<picojson::array>().size() == 0) {
            properties props;
            props.set("virtual", true);
            auto& added = reference.add(props);
            added.flags |= type::entity::VIRTUAL;
            added.position.reposition({ 0.0f, -100.0f, 0.0f });
            added.update();
        }
        else {
            for (auto& instance : entity.second.get("instances").get<picojson::array>()) {
                auto& position = instance.get("position");
                spatial::vector::type_t x = get(position, "x");
                spatial::vector::type_t y = get(position, "y");
                spatial::vector::type_t z = get(position, "z");

                properties props;
                if (instance.contains("properties")) {
                    parseProperties(instance.get("properties"), props);
                }

                auto& added = reference.add(props);
                if (props.has("spin")) {
                    added.position.spin(std::get<double>(props.get("spin")));
                }
                if (props.has("scale")) {
                    added.position.scale(std::get<double>(props.get("scale")));
                }
                added.position.reposition({ x, y, z });

                if (props.flag("virtual")) {
                    added.flags |= type::entity::VIRTUAL;
                }
                added.update();

                if (reference.flag("terrain") == false) {
                    stage::scene::global().map.setQuadrant(stage::scene::global().map.getQuadrant(x, z), stage::scene::global().map.getGenericType());
                }
            }
        }
        if (reference.instances.size()) {
            stage::scene::global().call("/play " + entity.first + " static");
        }

        percentage += increment;
        stage::scene::global().progress.value.set(percentage);
    }

    for (auto& group : groups) {
        auto& reference = assets->get<type::group>(group.first);
        parseProperties(group.second, reference);
        auto source = group.second.get("property").to_str();
        for (auto& entity : assets->get<type::entity>()) {
            for (auto& instance : entity->instances) {
                if (instance.second.has(source) && std::get<std::string>(instance.second.get(source)) == group.first) {
                    reference.add(instance.first);
                }
            }
        }
        percentage += increment;
        stage::scene::global().progress.value.set(percentage);
    }

    for (auto& blueprint : blueprints) {
        auto& reference = assets->get<type::blueprint>(blueprint.first);
        parseProperties(blueprint.second, reference);
        auto source = blueprint.second.get("property").to_str();
        for (auto& entity : assets->get<type::entity>()) {
            for (auto& instance : entity->instances) {
                if (instance.second.has(source) && std::get<std::string>(instance.second.get(source)) == blueprint.first) {
                    reference.add(instance.first);
                }
            }
        }
        percentage += increment;
        stage::scene::global().progress.value.set(percentage);
    }

    return true;
}


stage::scene::persistence::persistence() {
    _file = filesystem->join({ filesystem->appdata(), "map.json" });
}

bool stage::scene::persistence::write() {
    auto path = filesystem->dirname(_file);
    if (path.empty()) {
        return false;
    }

    if (filesystem->mkpath(path) == false) {
        // TODO capture errors
        //console::trace() << "failed creating output directory";
        return false;
    }

    picojson::object output;
    picojson::object entities;

    for (auto& entity : assets->get<type::entity>()) {
        picojson::object spec;
        picojson::array instances;
        for (auto& instance : entity->instances) {
            // Skip all non-user created map content
            if(entity->flag("required") == false && instance.second.flag("user") == false && instance.second.flag("required") == false) {
                continue;
            }
            picojson::object entry;
            if (instance.second.position.translation.spin != 0.0f) {
                instance.second.set("spin", instance.second.position.translation.spin);
            }
            if (instance.second.position.translation.scale != 1.0f) {
                instance.second.set("scale", instance.second.position.translation.scale - 1.0f);
            }
            picojson::object position;
            position["x"] = picojson::value(instance.second.position.eye.x);
            position["y"] = picojson::value(instance.second.position.eye.y);
            position["z"] = picojson::value(instance.second.position.eye.z);
            entry["position"] = picojson::value(position);

            picojson::object spec;
            if (writeProperties(instance.second, spec)) {
                entry["properties"] = picojson::value(spec);
            }

            instances.push_back(picojson::value(entry));
        }
        if (instances.size() == 0) {
            continue;
        }

        spec["instances"] = picojson::value(instances);

        picojson::object properties;
        if (writeProperties(*entity, properties)) {
            spec["properties"] = picojson::value(properties);
        }

        entities[entity->id()] = picojson::value(spec);
    }
    for (auto& entity : assets->list("objects", "directory")) {
        std::string id = "objects\/" + entity;
        if (entities.find(id) == entities.end()) {
            picojson::object entry;
            picojson::object spec;
            picojson::array instances;
            {
                picojson::object instance;
                picojson::object spec;
                picojson::object position;
                position["x"] = picojson::value(0.0);
                position["y"] = picojson::value(0.0);
                position["z"] = picojson::value(0.0);
                instance["position"] = picojson::value(position);
                spec["virtual"] = picojson::value(true);
                instance["properties"] = picojson::value(spec);
                instances.push_back(picojson::value(instance));
            }
            if (assets->has<type::entity>(id)) {
                writeProperties(assets->get<type::entity>(id), spec);
            }
            entry["instances"] = picojson::value(instances);
            entry["properties"] = picojson::value(spec);
            entities[id] = picojson::value(entry);
        }
    }

    picojson::object groups;
    //for (auto entry : assets->get<type::group>()) {
    //    picojson::object group;
    //    if (writeProperties(*entry, group)) {
    //        groups[entry->id()] = picojson::value(group);
    //    }
    //}
    picojson::object blueprints;
    for (auto entry : assets->get<type::blueprint>()) {
        picojson::object blueprint;
        if (writeProperties(*entry, blueprint)) {
            blueprints[entry->id()] = picojson::value(blueprint);
        }
    }
    output["entities"] = picojson::value(entities);
    output["groups"] = picojson::value(groups);
    output["blueprints"] = picojson::value(blueprints);

    try {
        //std::ofstream stream("/data/local/tmp/" + _file);
        std::ofstream stream(_file);
        stream << picojson::value(output).serialize();
        stream.close();
        return true;
    }
    catch (...) {
        // TODO capture errors
        //console::trace() << "failed writing output";
    }
    return false;
}

bool stage::scene::persistence::read() {
    std::stringstream buffer;
    if (filesystem->exists(_file)) {
        std::ifstream stream(_file);
        buffer << stream.rdbuf();
    }
    else {
        buffer << assets->retrieve(utilities::basename(_file)).rdbuf();
    }
    try {
        return parse(buffer.str());
    }
    catch (...) {
        // TODO capture errors
        //console::trace() << "failed reading input";
    }
    return false;
}


void stage::scene::add(std::string name, handler* instance) {
    std::lock_guard<std::mutex> scoped(lock);
    scenes[name] = instance;
}

bool stage::scene::load(std::string name) {
    scenes[name]->loaded = scenes[name]->load();
    return scenes[name]->loaded;
}

bool stage::scene::toggle(std::string name) {
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

bool stage::scene::isactive(std::string name) {
    std::lock_guard<std::mutex> scoped(lock);
    if (name.empty() || scenes.find(name) == scenes.end()) {
        return false;
    }
    return (active.find(name) != active.end());
}

bool stage::scene::activate(std::string name) {
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

bool stage::scene::deactivate(std::string name) {
    std::lock_guard<std::mutex> scoped(lock);
    if (name.empty() || active.find(name) == active.end()) {
        return false;
    }
    auto reference = active[name];
    active.erase(name);
    reference->stop();
    return true;
}

bool stage::scene::transition(std::string from, std::string to) {
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

void stage::scene::draw() {
    auto list = active;
    for (auto scene : list) {
        scene.second->draw();
    }
}

void stage::scene::run() {
    if (transitions.size()) {
        transition(transitions.front().first, transitions.front().second);
    }
    auto list = active;
    for (auto scene : list) {
        scene.second->run();
    }
}

std::map<std::string, stage::scene::handler*> stage::scene::current() {
    return active;
}

/// <summary>
/// Event propagation to the active scenes
/// </summary>

void stage::scene::freelook_start(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_start(ev);
    }
}
void stage::scene::freelook_move(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_move(ev);
    }
}
void stage::scene::freelook_zoom(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->freelook_zoom(ev);
    }
}
void stage::scene::mouse_click(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_click(ev);
    }
}
void stage::scene::mouse_move(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_move(ev);
    }
}
void stage::scene::mouse_drag(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_drag(ev);
    }
}
void stage::scene::mouse_release(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_release(ev);
    }
}
void stage::scene::mouse_scroll(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->mouse_scroll(ev);
    }
}
void stage::scene::keyboard_input(const platform::input::event& ev) {
    if (ev.gesture == platform::input::UP && ev.identifier == 192) {
        toggle("debug");
    }
    for (auto scene : current()) {
        scene.second->keyboard_input(ev);
    }
}
void stage::scene::gamepad_input(const platform::input::event& ev) {
    for (auto scene : active) {
        scene.second->gamepad_input(ev);
    }
}

void stage::scene::dimensions(int width, int height) {
    std::lock_guard<std::mutex> scoped(lock);
    for (auto scene : current()) {
        scene.second->dimensions(width, height);
    }
}

value_t stage::scene::call(const std::string& input) {
    auto tokens = utilities::tokenize(input, " ");
    if (tokens.empty() || commands.find(tokens[0]) == commands.end()) {
        std::vector<std::string> list;
        for (auto command : commands) {
            list.push_back(command.first);
        }
        std::stringstream ss;
        ss << "commands: " << utilities::join(", ", list);
        stage::scene::global().debug.content.add(ss.str());
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
    stage::scene::global().debug.content.add(ss.str());

    return result;
}

value_t stage::scene::set(label_t label, value_t value) {
    properties::set(label, value);
    if (events.find(label) != events.end()) {
        events[label](parameters_t()); // TODO: actually pass in the value
    }
    return value;
}

value_t stage::scene::_group(parameters_t p) {
    if (p.size() < 2) {
        stage::scene::global().debug.content.add(commands["group"].first);
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
        assets->get<type::group>(group).set(std::get<label_t>(p[1]), p[2]);
        for (auto scene : active) {
            scene.second->group(group, std::get<label_t>(p[1]), p[2]);
        }
    }
    return p[1];
}

value_t stage::scene::_get(parameters_t p) {
    if (p.size() < 1) {
        stage::scene::global().debug.content.add(commands["get"].first);
        return 0;
    }
    return properties::get(std::get<label_t>(p[0]));
}

value_t stage::scene::_set(parameters_t p) {
    if (p.size() < 2) {
        stage::scene::global().debug.content.add(commands["set"].first);
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

value_t stage::scene::_load(parameters_t p) {
    if (p.size() == 2) {
        assets->load(std::get<std::string>(p[0]), std::get<std::string>(p[1]));
    }
    if (p.size() == 3) {
        assets->load(std::get<std::string>(p[0]), std::get<std::string>(p[1]), std::get<std::string>(p[2]));
    }
    while (assets->events()) {
        stage::scene::global().debug.content.add(assets->event());
    }
    return 0;
}

value_t stage::scene::_compile(parameters_t p) {
    graphics->compile(assets);
    while (graphics->events()) {
        stage::scene::global().debug.content.add(graphics->event());
    }
    return 0;
}

value_t stage::scene::_play(parameters_t p) {
    if (p.size() == 2) {
        auto name = std::get<label_t>(p[0]);
        auto animation = std::get<label_t>(p[1]);
        assets->get<type::entity>(name).play(animation);
        return 1;
    }
    return 0;
}

value_t stage::scene::_create(parameters_t p) {
    return 0;
}

value_t stage::scene::_show(parameters_t p) {
    if (p.size() == 0) {
        stage::scene::global().debug.content.add("/show [commands|variables|assets|loaded|entities]");
        return 0;
    }

    auto type = std::get<std::string>(p[0]);
    if (type == "objects") {
        for (auto entry : assets->get<type::object>()) {
            std::stringstream ss;
            ss << "objects(" << entry->id() << ")";
            stage::scene::global().debug.content.add(ss.str());
        }
    }
    else if (type == "entity") {
        if (p.size() != 2) {
            stage::scene::global().debug.content.add("/show entity [path]");
            return 0;
        }
        auto path = std::get<std::string>(p[1]);
        properties* entry = &assets->get<type::entity>(path);
        for (auto prop : entry->keys()) {
            std::pair<std::string, value_t> variable = { prop, entry->get(prop) };
            std::stringstream ss;
            if (std::holds_alternative<bool>(variable.second)) {
                ss << variable.first << ": " << (std::get<bool>(variable.second) ? "true" : "false");
            }
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
            stage::scene::global().debug.content.add(ss.str());
        }
    }
    else if (type == "entities") {
        for (auto entry : assets->get<type::entity>()) {
            std::stringstream ss;
            ss << "entity(" << entry->id() << ", " << entry->instances.size() << ")";
            stage::scene::global().debug.content.add(ss.str());
        }
    }
    else if (type == "variables") {
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
            stage::scene::global().debug.content.add(ss.str());
        }
    }
    else if (type == "commands") {
        for (auto command : commands) {
            stage::scene::global().debug.content.add(command.second.first);
        }
    }
    else if (type == "assets") {
        if (p.size() != 2) {
            stage::scene::global().debug.content.add("/show assets [path]");
            return 0;
        }
        auto path = std::get<std::string>(p[1]);
        for (auto asset : assets->list(path)) {
            stage::scene::global().debug.content.add(asset);
        }
    }
    else {
        auto list = active;
        for (auto scene : list) {
            scene.second->show(type);
        }
    }

    return 0;
}

value_t stage::scene::_save(parameters_t p) {
    auto list = active;
    for (auto scene : list) {
        scene.second->save();
    }
    return 0;
}

value_t stage::scene::_exit(parameters_t p) {
    stage::scene::global().debug.content.add("goodbye");
    ::exit(0);
}

void _output_test_results(std::string entry, std::pair<bool, std::string> results) {
    if (results.first) {
        stage::scene::global().debug.content.add("[ PASSED ] " + entry); 
    }
    else {
        stage::scene::global().debug.content.add("[ FAILED ] " + entry);
        stage::scene::global().debug.content.add("... " + results.second);
    }
}

value_t stage::scene::_test(parameters_t p) {
    if (p.size() >= 1) {
        auto command = std::get<std::string>(p[0]);
        if (command == "run") {
            stage::scene::global().debug.content.add("running test");
            if (p.size() == 2) {
                auto entry = std::get<std::string>(p[1]);
                _output_test_results(entry, tests->run(entry));
            }
            else for (auto entry : tests->list()) {
                _output_test_results(entry, tests->run(entry));
            }
        }
        if (command == "list") {
            for (auto entry : tests->list()) {
                stage::scene::global().debug.content.add("  " + entry);
            }
            return (int)tests->list().size();
        }
    }
    else {
        stage::scene::global().debug.content.add("/test <command> [group]");
    }
    return 0;
}
