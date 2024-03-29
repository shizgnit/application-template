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

#if defined __PLATFORM_UNIVERSAL

void implementation::universal::input::raise(const event& ev) {
    // Dispatch the event based on type
    if (ev.input == GAMEPAD) {
        if (ev.gesture == DOWN || ev.gesture == HELD) {
            on_button_down(ev);
        }
        if (ev.gesture == UP) {
            on_button_up(ev);
        }
    }
    if (ev.input == POINTER) {
        if (ev.gesture == DOWN) {
            on_press(ev);
        }
        if (ev.gesture == UP) {
            on_release(ev);
        }
        if (ev.gesture == MOVE) {
            on_move(ev);
        }
        if (ev.gesture == WHEEL) {
            platform::input::raise({ POINTER, WHEEL, active_pointer(), time(NULL) - platform::pointers[ev.identifier].pressed, ev.travel, ev.point });
        }
    }
    if (ev.input == KEY) {
        if (ev.gesture == DOWN) {
            on_key_down(ev);
        }
        if (ev.gesture == UP) {
            on_key_up(ev);
        }
    }
}
int implementation::universal::input::active_pointer() {
    return active_pointer(event());
}
int implementation::universal::input::active_pointer(const event& ev) {
    auto derived = ev.identifier;
    for (int i = 0; i < 8; i++) {
        if (derived == 0 && platform::pointers[i].pressed) {
            derived = i;
            break;
        }
    }
    // Special case to simulate middle mouse... not sure if
    if (derived == 0 && platform::pointers[1].pressed && platform::pointers[2].pressed) {
        derived = 3;
    }
    return derived;
}

void implementation::universal::input::on_press(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - platform::pointers[ev.identifier].pressed;
    float distance = platform::pointers[ev.identifier].point.distance(ev.point);

    // Update to current
    if (platform::pointers[ev.identifier].pressed == 0) {
        platform::pointers[ev.identifier].pressed = time(NULL);
    }
    platform::pointers[ev.identifier].point = ev.point;

    // Track the event
    active_pointers.push_back(&platform::pointers[ev.identifier]);

    drag = true;
    
    // TODO: make the thresholds configurable
    if (delta <= 1 && distance < 1.0) {
        platform::input::raise({ POINTER, DOUBLE, ev.identifier, delta, 0.0f, ev.point });
    }
    else {
        platform::input::raise({ POINTER, DOWN, ev.identifier, delta, 0.0f, ev.point });
    }
}

void implementation::universal::input::on_release(const event& ev) {
    auto end = std::remove_if(active_pointers.begin(),
                              active_pointers.end(),
                              [ev](pointer const* p) {
                                return p->code == ev.identifier;
                              });
    active_pointers.erase(end, active_pointers.end());

    // Store off the pressed so it can be cleared before raising the event
    auto reference = platform::pointers[ev.identifier].pressed;
    platform::pointers[ev.identifier].pressed = 0;

    platform::input::raise({ POINTER, drag ? RELEASE : UP, ev.identifier, time(NULL) - reference, 0.0f, ev.point, points });
    points.clear();
}

void averages(const implementation::universal::input::event& ev, float& distance, spatial::vector& point) {
    float total_distance = 0.0f;
    if (ev.points.empty()) {
        point = ev.point;
        return;
    }

    point = ev.points[0];
    std::for_each(std::begin(ev.points) + 1, std::end(ev.points), [ev, &total_distance, &point](const spatial::vector p) {
        total_distance += ev.points[0].distance(p);
        point.x += p.x;
        point.y += p.y;
        point.z += p.z;
        });

    distance = total_distance / ev.points.size();
    point.x /= ev.points.size();
    point.y /= ev.points.size();
    point.z /= ev.points.size();
}

void implementation::universal::input::on_move(const event& ev) {
    if (active_pointers.size()) {
        auto active = active_pointer(ev);
        if (ev.points.size() > 1) {
            drag = false; // Don't allow trailing input to drag the view
            float current_distance;
            spatial::vector current_position;

            averages(ev, current_distance, current_position);
            points.push_back(current_position);

            if (abs(current_distance - last_distance) > threshold_travel) {
                platform::input::raise({ POINTER, PINCH, active, time(NULL) - platform::pointers[ev.identifier].pressed, current_distance - last_distance, current_position, ev.points });
            }
            else {
                platform::input::raise({ POINTER, WHEEL, active, time(NULL) - platform::pointers[ev.identifier].pressed, last_position.distance(current_position), current_position, ev.points});
            }

            last_distance = current_distance;
            last_position = current_position;
        }
        else if(drag) {
            points.push_back(ev.point);
            platform::input::raise({ POINTER, DRAG, active, time(NULL) - platform::pointers[ev.identifier].pressed, 0.0f, ev.point, points });
        }
    }
    else {
        drag = true;
        averages(ev, last_distance, last_position);
        platform::input::raise({ POINTER, MOVE, ev.identifier, time(NULL) - platform::pointers[ev.identifier].pressed, 0.0f, ev.point });
    }
}

void implementation::universal::input::on_key_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - platform::pointers[ev.identifier].pressed;

    // Update to current
    platform::keys[ev.identifier].pressed = time(NULL);

    // Track the event
    tracking.lock();
    active_keys.push_back(&platform::keys[ev.identifier]);
    tracking.unlock();

    platform::input::raise({ KEY, DOWN, ev.identifier, delta, 0.0f, { 0.0f, 0.0f, 0.0f } });
}

void implementation::universal::input::on_key_up(const event& ev) {
    if (active_keys.size()) {
        tracking.lock();
        auto end = std::remove_if(active_keys.begin(),
            active_keys.end(),
            [ev](key const* k) {
                return k->code == ev.identifier;
            });
        active_keys.erase(end, active_keys.end());
        tracking.unlock();
    }

    // Store off the pressed so it can be cleared before raising the event
    auto reference = platform::keys[ev.identifier].pressed;
    platform::keys[ev.identifier].pressed = 0;

    platform::input::raise({ KEY, UP, ev.identifier, time(NULL) - reference, 0.0f, { 0.0f, 0.0f, 0.0f } });
}

void implementation::universal::input::on_button_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - platform::pointers[ev.identifier].pressed;

    // Update to current
    platform::buttons[ev.identifier].pressed = time(NULL);

    // Track the event
    tracking.lock();
    active_buttons.push_back(&platform::buttons[ev.identifier]);
    tracking.unlock();

    platform::input::raise({ GAMEPAD, DOWN, ev.identifier, delta, 0.0f, { 0.0f, 0.0f, 0.0f } });
}

void implementation::universal::input::on_button_up(const event& ev) {
    if (active_buttons.size()) {
        tracking.lock();
        auto end = std::remove_if(active_buttons.begin(),
            active_buttons.end(),
            [ev](gamepad const* b) {
                return b->code == ev.identifier;
            });
        active_buttons.erase(end, active_buttons.end());
        tracking.unlock();
    }

    platform::keys[ev.identifier].pressed = 0;
    platform::input::raise({ GAMEPAD, UP, ev.identifier, time(NULL) - platform::buttons[ev.identifier].pressed, 0.0f, { 0.0f, 0.0f, 0.0f } });
}

void implementation::universal::input::emit() {
    for (auto active : active_pointers) {
        time_t delta = time(NULL) - active->pressed;
        platform::input::raise({ POINTER, HELD, active->code, delta, 0.0f, active->point });
    }
    for (auto active : active_keys) {
        time_t delta = time(NULL) - active->pressed;
        platform::input::raise({ KEY, HELD, active->code, delta, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
}

bool implementation::universal::interface::raise(const input::event& ev, int x, int y) {
    widget* target = NULL;
    if (ev.input == input::POINTER) {
        //spatial::vector relative = { (float)x, (float)(graphics->height() - y), 0.0f };
        spatial::vector relative = { (float)x, (float)y, 0.0f };
        spatial::vector position = relative.project(spatial::matrix(), spatial::matrix(), spatial::matrix());
        spatial::ray ray(position - spatial::vector(0,0,100), position - spatial::vector(0,0,-100));

        // See if any widgets were selected
        for (auto instance : instances) {
            if (instance.second->enabled && instance.second->visible && ray.intersects(instance.second->bounds)) {
                target = instance.second;
            }
        }
        if(ev.gesture == platform::input::DOWN) {
            select(target);
        }
        // Pass along the events
        if (target) {
            target->events.raise(ev);
            return target->passthrough == false;
        }
    }

    if (ev.input == input::KEY && selected) {
        selected->events.raise(ev);
        return selected == NULL || selected->passthrough == false;
    }

    return false;
}

void implementation::universal::interface::emit() {

}

void implementation::universal::interface::dimensions(int width, int height) {
    if (display_width != width || display_height != height) {
        projection = spatial::matrix().ortho(0, width, 0, height);
        display_width = width;
        display_height = height;
    }
    for (auto instance : instances) {
        position(*instance.second);
    }
}

platform::interface::widget* implementation::universal::interface::reposition(std::vector<widget*>& c) {
    if (config.spec == interface::widget::spec::none) {
        return NULL;
    }

    for (auto w = c.begin(); w != c.end(); w++) {
        int size = std::distance(c.begin(), w) + 1;

        int target_x = config.x;
        int target_y = config.y;

        int offset = (config.margin * size) + config.margin;
        if (config.relativity == interface::widget::positioning::bottom) {
            target_y += (config.h * size) + offset;
        }
        if (config.relativity == interface::widget::positioning::right) {
            target_x += (config.w * size) + offset;
        }

        (*w)->position(target_x, target_y);
    }
    return c.back();
}

void implementation::universal::interface::draw() {
    for (auto instance : instances) {
        draw(*instance.second);
    }
}

platform::interface::widget* implementation::universal::interface::create(std::vector<widget*>& c) {
    if (config.spec == interface::widget::spec::none) {
        return NULL;
    }

    int target_x = config.x;
    int target_y = config.y;

    int offset = (config.margin * c.size()) + config.margin;
    if (config.relativity == interface::widget::positioning::bottom) {
        target_y += (config.h * c.size()) + offset;
    }
    if (config.relativity == interface::widget::positioning::right) {
        target_x += (config.w * c.size()) + offset;
    }

    c.push_back(create(config.spec, config.w, config.h, config.background.r, config.background.g, config.background.b, config.background.a));
    c.back()->position(target_x, target_y);

    return c.back();
}

platform::interface::widget* implementation::universal::interface::create(platform::interface::widget::spec t, int w, int h, int r, int g, int b, int a) {
    widget* instance = NULL;

    switch (t) {
    case(widget::spec::button):
        instance = new button();
        break;
    case(widget::spec::textbox):
        instance = new textbox();
        break;
    case(widget::spec::progress):
        instance = new progress();
        break;
    }

    return create(instance, w, h, r, g, b, a);
}

platform::interface::widget* implementation::universal::interface::create(widget* instance, int w, int h, int r, int g, int b, int a) {
    instance->background = spatial::quad(w, h);
    instance->background.texture.create(1, 1, r, g, b, a); // Single pixel is good enough
    instance->background.xy_projection(0, 0, w, h);

    int l = 100;

    instance->edge = spatial::quad::edges(w, h);
    instance->edge.texture.create(1, 1, r+l, g+l, b+l, a+l);
    instance->edge.xy_projection(0, 0, 1, 1);

    instance->bounds = spatial::quad(instance->background.vertices).project(spatial::matrix(), spatial::matrix(), projection);

    instance->visible = false;

    instances[instance->id] = instance;

    return instance;
}

void implementation::universal::interface::print(int x, int y, const std::string& text) {
    spatial::matrix position;
    position.identity();
    position.translate(x, (graphics->height() - (font.height() * gui->scale())) - y, 0);
    position.scale(gui->scale());

    graphics->draw(text, font, shader, projection, spatial::matrix(), position);
}

spatial::vector implementation::universal::interface::placement() {
    graphics->scale() + config.x;
    return { 0.0, 0.0, 0.0, 0.0 };
}

void implementation::universal::interface::position(widget& instance) {
    spatial::matrix position;
    //position.translate(instance.x, (graphics->height() - instance.background.height()) - instance.y, 0);
    position.translate(instance.x, instance.y, 0);
    instance.bounds = position.interpolate(spatial::quad(instance.background.vertices));
}

void implementation::universal::interface::draw(widget& instance) {
    if (instance.visible == false) {
        return;
    }

    graphics->clip(graphics->height() - instance.y, -((graphics->height() - instance.background.height()) - instance.y), -instance.x, instance.x + instance.background.width());

    spatial::matrix position;
    position.translate(instance.x, (graphics->height() - instance.background.height()) - instance.y, 0);

    int left_margin = 10;

    spatial::matrix edge = position; // Save off the scaling to keep proportions

    if (instance.specification == widget::spec::progress) {
        platform::interface::progress& progress = gui->cast<platform::interface::progress>(instance);
        position.scale_x(progress.value.get() / 100.0f);
    }

    if (instance.background.visible && instance.background.vertices.size()) {
        graphics->draw(instance.background, shader, projection, spatial::matrix(), position);
    }
    if (instance.foreground.visible && instance.foreground.vertices.size()) {
        graphics->draw(instance.foreground, shader, projection, spatial::matrix(), position);
    }
    if (instance.edge.visible && instance.edge.vertices.size()) {
        graphics->draw(instance.edge, shader, projection, spatial::matrix(), edge, spatial::matrix(), platform::graphics::render::WIREFRAME);
    }

    if (instance.specification == widget::spec::panel) {
        platform::interface::panel& panel = gui->cast<platform::interface::panel>(instance);
        for (auto& child : panel.children) {
            draw(*child);
        }
    }
    if (instance.specification == widget::spec::progress) {
        platform::interface::progress& progress = gui->cast<platform::interface::progress>(instance);
        std::stringstream ss;
        ss << progress.value.get() << "%";
        print(progress.x + left_margin, progress.y, ss.str());
    }
    if (instance.specification == widget::spec::textbox) {
        platform::interface::textbox& textbox = gui->cast<platform::interface::textbox>(instance);
        auto contents = textbox.content.get();

        int x = textbox.x + left_margin;
        int y = textbox.alignment == widget::positioning::bottom ? textbox.y + textbox.background.height() - (contents.size() * font.leading()) : textbox.y;

        for (auto message : contents) {
            if (instance.input && selected == &instance && time(NULL) % 2 == 0) {
                message += "|";
            }
            if (message.empty() == false) {
                print(x, y, message);
                y += font.leading();
            }
        }
    }

    graphics->noclip();
}

std::string implementation::universal::assets::load(platform::assets* instance, const std::string& type, const std::string& resource, const std::string& id) {
    auto path = resource;
    if (has(type + ".path")) {
        auto source = get(type + ".path");
        path = std::get<std::string>(source) + "/" + resource;
    }

    int dot = path.find_last_of(".");
    int slash = path.find_last_of("/");

    std::string ext = "";
    if (dot != std::string::npos) {
        int len = path.length() - dot;
        if (utilities::numeric(path.substr(dot, len)) == false) {
            ext = path.substr(dot, len);
            path = path.substr(0, path.length() - len);
        }
    }

    std::string cache = id.empty() ? path : id;

    if (type == "material") {
        instance->retrieve(path + (ext.empty() ? ".mtl" : ext)) >> format::parser::mtl >> instance->get<type::material>(cache);
    }
    if (type == "texture") {
        instance->retrieve(path + (ext.empty() ? ".png" : ext)) >> format::parser::png >> instance->get<type::image>(cache);
    }
    if (type == "audio") {
        instance->retrieve(path + (ext.empty() ? ".wav" : ext)) >> format::parser::wav >> instance->get<type::sound>(cache);
    }
    if (type == "shader") {
        auto& shader = instance->get<type::program>(cache);
        instance->retrieve(path + ".vert") >> format::parser::vert >> shader.vertex;
        instance->retrieve(path + ".frag") >> format::parser::frag >> shader.fragment;
        //instance->retrieve(path + ".metal") >> format::parser::metal >> shader.unified;
        
        if(has("shader.version")) {
            std::string version = std::get<std::string>(get("shader.version"));
            if(shader.vertex.text.empty() == false) {
                shader.vertex.text = version + "\n" + shader.vertex.text;
            }
            if(shader.fragment.text.empty() == false) {
                shader.fragment.text = version + "\n" + shader.fragment.text;
            }
        }
    }
    if (type == "font") {
        instance->retrieve(path + (ext.empty() ? ".fnt" : ext)) >> format::parser::fnt >> instance->get<type::font>(cache);
    }
    if (type == "object") {
        if (ext == ".fbx") {
            instance->retrieve(path + ext) >> format::parser::fbx >> instance->get<type::object>(cache);
        }
        else if (ext == ".obj") {
            instance->retrieve(path + ext) >> format::parser::obj >> instance->get<type::object>(cache);
        }
        else if (ext == ".png") {
            auto& object = instance->get<type::object>(cache);
            object.texture.color = &instance->get<type::image>(cache);
            instance->retrieve(path + ext) >> format::parser::png >> *object.texture.color;
            object = spatial::quad(object.texture.color->properties.width, object.texture.color->properties.height);
            object.xy_projection(0, 0, object.texture.color->properties.width, object.texture.color->properties.height);
        }
        else {
            instance->retrieve(path + (ext.empty() ? ".obj" : ext)) >> format::parser::obj >> instance->get<type::object>(cache);
        }
    }
    if (type == "entity") {
        auto& entity = instance->get<type::entity>(cache);

        std::vector<std::string> states;
        
        std::string object_name;
        std::string icon_name;

        for (auto resource : instance->list(path)) {
            auto ext = utilities::extension(resource);
            if (ext.empty() == false) {
                if (ext == "obj") {
                    object_name = resource;
                }
                if (resource == "icon.png") {
                    icon_name = resource;
                }
            }
            else {
                states.push_back(resource);
            }
        }

        spatial::vector offset;
        if (object_name.empty() == false) {
            entity.animations["static"].frames.resize(1);
            instance->retrieve(path + "/" + object_name) >> format::parser::obj.d(resource + ".") >> entity.animations["static"].frames[0];
            entity.object = &entity.animations["static"].frames[0];

            if (entity.has("scale")) {
                auto scale = std::get<double>(entity.get("scale"));
                entity.object->scale(spatial::matrix().scale(scale));
            }

            if (entity.has("center") == false || std::get<bool>(entity.get("center")) == true) {
                auto center = entity.object->center();
                auto min = entity.object->min();
                offset = {
                     -center.x,
                     -min.y,
                     -center.z
                };
                entity.object->reposition(offset);
            }

            if (icon_name.empty() == false) {
                auto cache = path + "/" + icon_name;
                auto& icon = instance->get<type::object>(cache);
                entity.object->icon = &icon;
                icon.texture.color = &instance->get<type::image>(cache);
                instance->retrieve(cache) >> format::parser::png >> icon.texture.color;
                icon = spatial::quad(icon.texture.color->properties.width, icon.texture.color->properties.height);
                icon.xy_projection(0, 0, icon.texture.color->properties.width, icon.texture.color->properties.height);
            }
        }

        for (auto state : states) {
            std::vector<std::string> resources;
            
            resources = instance->list(path + "/" + state);
            std::sort(resources.begin(), resources.end());

            std::vector<std::string> objects;
            for (auto resource : resources) {
                if (utilities::extension(resource) == "obj") {
                    objects.push_back(resource);
                }
            }

            entity.animations[state].frames.resize(objects.size());

            int frame = 0;
            for (auto resource : objects) {
                const char* r = std::string(path + "/" + state + "/" + resource).c_str();
                instance->retrieve(path + "/" + state + "/" + resource) >> format::parser::obj.d(resource + ".").o(offset) >> entity.animations[state].frames[frame];
                frame++;
            }
        }
    }

    return cache;
}

#endif
