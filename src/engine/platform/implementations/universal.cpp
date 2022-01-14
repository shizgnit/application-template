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
            platform::input::raise({ POINTER, WHEEL, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, ev.travel, ev.point });
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

void implementation::universal::input::on_press(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - pointers[ev.identifier].pressed;
    float distance = pointers[ev.identifier].point.distance(ev.point);

    // Update to current
    pointers[ev.identifier].pressed = time(NULL);
    pointers[ev.identifier].point = ev.point;

    // Track the event
    active_pointers.push_back(&pointers[ev.identifier]);

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
    platform::input::raise({ POINTER, UP, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, 0.0f, ev.point });
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
        if (ev.points.size() > 1) {
            drag = false; // Don't allow trailing input to drag the view
            float current_distance;
            spatial::vector current_position;

            averages(ev, current_distance, current_position);

            if (abs(current_distance - last_distance) > threshold_travel) {
                platform::input::raise({ POINTER, PINCH, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, current_distance - last_distance, current_position, ev.points });
            }
            else {
                platform::input::raise({ POINTER, WHEEL, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, last_position.distance(current_position), current_position, ev.points});
            }

            last_distance = current_distance;
            last_position = current_position;
        }
        else if(drag) {
            platform::input::raise({ POINTER, DRAG, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, 0.0f, ev.point });
        }
    }
    else {
        drag = true;
        averages(ev, last_distance, last_position);
        platform::input::raise({ POINTER, MOVE, ev.identifier, time(NULL) - pointers[ev.identifier].pressed, 0.0f, ev.point });
    }
}

void implementation::universal::input::on_key_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - pointers[ev.identifier].pressed;

    // Update to current
    keys[ev.identifier].pressed = time(NULL);

    // Track the event
    tracking.lock();
    active_keys.push_back(&keys[ev.identifier]);
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

    keys[ev.identifier].pressed = 0;
    platform::input::raise({ KEY, UP, ev.identifier, time(NULL) - keys[ev.identifier].pressed, 0.0f, { 0.0f, 0.0f, 0.0f } });
}

void implementation::universal::input::on_button_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - pointers[ev.identifier].pressed;

    // Update to current
    buttons[ev.identifier].pressed = time(NULL);

    // Track the event
    tracking.lock();
    active_buttons.push_back(&buttons[ev.identifier]);
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

    keys[ev.identifier].pressed = 0;
    platform::input::raise({ GAMEPAD, UP, ev.identifier, time(NULL) - buttons[ev.identifier].pressed, 0.0f, { 0.0f, 0.0f, 0.0f } });
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

void implementation::universal::interface::raise(const input::event& ev, int x, int y) {
    spatial::vector relative = { (float)x, (float)(graphics->height() - y), 0.0f };
    spatial::vector position = relative.project(spatial::matrix(), spatial::matrix(), spatial::matrix());
    spatial::ray ray(position - spatial::vector(0,0,100), position - spatial::vector(0,0,-100));

    widget* target = NULL;
    if (ev.input == input::POINTER && ev.gesture == platform::input::DOWN) {
        // See is any widgets were selected
        for (auto instance : instances) {
            if (instance.second->enabled && instance.second->visible && ray.intersects(instance.second->bounds)) {
                target = instance.second;
            }
        }
        select(target);
        // Pass along the events
        if (target) {
            target->raise(ev);
        }
    }

    if (ev.input == input::KEY && selected) {
        selected->raise(ev);
    }
}

void implementation::universal::interface::emit() {

}

void implementation::universal::interface::position() {
    for (auto instance : instances) {
        position(*instance.second);
    }
}

void implementation::universal::interface::draw() {
    for (auto instance : instances) {
        draw(*instance.second);
    }
}

platform::interface::widget& implementation::universal::interface::create(platform::interface::widget::spec t, int w, int h, int r, int g, int b, int a) {
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

platform::interface::widget& implementation::universal::interface::create(widget* instance, int w, int h, int r, int g, int b, int a) {
    instance->background = spatial::quad(w, h);
    instance->background.texture.map.create(1, 1, r, g, b, a); // Single pixel is good enough
    instance->background.xy_projection(0, 0, w, h);
    graphics->compile(instance->background);

    int l = 100;

    instance->edge = spatial::quad::edges(w, h);
    instance->edge.texture.map.create(1, 1, r+l, g+l, b+l, a+l);
    instance->edge.xy_projection(0, 0, 1, 1);
    graphics->compile(instance->edge);

    instance->bounds = spatial::quad(instance->background.vertices).project(spatial::matrix(), spatial::matrix(), projection);

    instances[instance->id] = instance;

    return *instance;
}

void implementation::universal::interface::print(int x, int y, const std::string& text) {
    spatial::matrix position;
    position.identity();
    position.scale(1.0f);
    position.translate(x, (graphics->height() - font.height()) - y, 0);

    graphics->draw(text, font, shader, position, spatial::matrix(), projection);
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

    graphics->draw(instance.background, shader, position, spatial::matrix(), projection);
    if (instance.edge.vertices.size()) {
        graphics->draw(instance.edge, shader, edge, spatial::matrix(), projection, platform::graphics::render::WIREFRAME);
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

        //std::string line;
        for (auto message : contents) {
            //line += message;
            if (instance.input && selected == &instance && time(NULL) % 2 == 0) {
                message += "|";
            }
            if (message.empty() == false) { //&& line[line.size() - 1] == '\n') {
                print(x, y, message);
                y += font.leading();
                //line.clear();
            }
        }
        //if (line.empty() == false) {
        //    print(x, y, line);
        //}
    }

    graphics->noclip();
}

void implementation::universal::interface::position(widget& instance) {
    spatial::matrix position;
    position.translate(instance.x, (graphics->height() - instance.background.height()) - instance.y, 0);
    instance.bounds = position.interpolate(spatial::quad(instance.background.vertices));
}

#endif
