#include "engine.hpp"

#if defined __PLATFORM_UNIVERSAL

void implementation::universal::input::raise(const event& ev) {
    // Dispatch the event based on type
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
            platform::input::raise({ POINTER, WHEEL, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
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
        platform::input::raise({ POINTER, DOUBLE_TAP, ev.identifier, ev.point, delta });
    }
    else {
        platform::input::raise({ POINTER, DOWN, ev.identifier, ev.point, delta });
    }
}

void implementation::universal::input::on_release(const event& ev) {
    auto end = std::remove_if(active_pointers.begin(),
                              active_pointers.end(),
                              [ev](pointer const* p) {
                                return p->code == ev.identifier;
                              });
    active_pointers.erase(end, active_pointers.end());
    platform::input::raise({ POINTER, UP, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
}

void implementation::universal::input::on_move(const event& ev) {
    if (active_pointers.size()) {
        if (active_pointers.size() > 1) { 
            // Note, this does not take into all permutations as it should
            // TODO: Add support for rotational gestures
            float total_distance = 0.0f;
            std::for_each(std::begin(active_pointers) + 1, std::end(active_pointers), [this, &total_distance](const pointer *p) {
                total_distance += active_pointers[0]->point.distance(p->point);
            });
            float average_distance = total_distance / (active_pointers.size() - 1);
            platform::input::raise({ POINTER, PINCH, ev.identifier, { average_distance, average_distance, 0.0f }, time(NULL) - pointers[ev.identifier].pressed });
        }
        else {
            platform::input::raise({ POINTER, DRAG, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
        }
    }
    else {
        platform::input::raise({ POINTER, MOVE, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
    }
}

void implementation::universal::input::on_key_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - pointers[ev.identifier].pressed;

    // Update to current
    keys[ev.identifier].pressed = time(NULL);

    // Track the event
    active_keys.push_back(&keys[ev.identifier]);

    platform::input::raise({ KEY, DOWN, ev.identifier, { 0.0f, 0.0f, 0.0f }, delta });
}

void implementation::universal::input::on_key_up(const event& ev) {
    auto end = std::remove_if(active_keys.begin(),
                              active_keys.end(),
                              [ev](key const* k) {
                                return k->code == ev.identifier;
                              });
    active_keys.erase(end, active_keys.end());
    input::raise({ KEY, UP, ev.identifier, { 0.0f, 0.0f, 0.0f }, time(NULL) - keys[ev.identifier].pressed });
}

void implementation::universal::input::emit() {
    for (auto active : active_pointers) {
        time_t delta = time(NULL) - active->pressed;
        platform::input::raise({ POINTER, HELD_DOWN, active->code, active->point, delta });
    }
    for (auto active : active_keys) {
        time_t delta = time(NULL) - active->pressed;
        platform::input::raise({ KEY, HELD_DOWN, active->code, { 0.0f, 0.0f, 0.0f }, delta });
    }
}

void implementation::universal::interface::raise(const input::event& ev) {

}

void implementation::universal::interface::emit() {

}

void implementation::universal::interface::draw() {
    for (auto instance : instances) {
        draw(*instance);
    }
}

platform::interface::widget& implementation::universal::interface::create(platform::interface::widget::type t, int w, int h, const std::string& texture) {
    switch (t) {
    case(widget::type::button):
        instances.push_back(new button());
        break;
    case(widget::type::textbox):
        instances.push_back(new textbox());
        break;
    }

    // TODO Always assuming a png is being specified for now, fix this
    assets->retrieve(texture) >> format::parser::png >> instances.back()->background.texture.map;

    instances.back()->background.quad(w, h);
    instances.back()->background.xy_projection(0, 0, w, h);

    graphics->compile(instances.back()->background);

    return *instances.back();
}

platform::interface::widget& implementation::universal::interface::create(platform::interface::widget::type t, int w, int h, int r, int g, int b, int a) {
    switch (t) {
    case(widget::type::button):
        instances.push_back(new button());
        break;
    case(widget::type::textbox):
        instances.push_back(new textbox());
        break;
    }

    instances.back()->background.texture.map.create(r, g, b, a);

    instances.back()->background.quad(w, h);
    instances.back()->background.xy_projection(0, 0, w, h);

    graphics->compile(instances.back()->background);

    return *instances.back();
}

void implementation::universal::interface::draw(widget& instance) {
    graphics->clip(graphics->height() - instance.y, -((graphics->height() - instance.background.height()) - instance.y), -instance.x, instance.x + instance.background.width());

    spatial::matrix position;
    position.identity();
    position.translate(instance.x, (graphics->height() - instance.background.height()) - instance.y, 0);

    graphics->draw(instance.background, shader, position, spatial::matrix(), projection);

    graphics->noclip();
}


#endif

