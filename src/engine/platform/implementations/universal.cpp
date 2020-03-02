#include "engine.hpp"

#if defined __PLATFORM_UNIVERSAL

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
        raise({ POINTER, DOUBLE_TAP, ev.identifier, ev.point, delta });
    }
    else {
        raise({ POINTER, DOWN, ev.identifier, ev.point, delta });
    }
}

void implementation::universal::input::on_release(const event& ev) {
    auto end = std::remove_if(active_pointers.begin(),
                              active_pointers.end(),
                              [ev](pointer const* p) {
                                return p->code == ev.identifier;
                              });
    active_pointers.erase(end, active_pointers.end());
    raise({ POINTER, UP, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
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
            raise({ POINTER, PINCH, ev.identifier, { average_distance, average_distance, 0.0f }, time(NULL) - pointers[ev.identifier].pressed });
        }
        else {
            raise({ POINTER, DRAG, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
        }
    }
    else {
        raise({ POINTER, MOVE, ev.identifier, ev.point, time(NULL) - pointers[ev.identifier].pressed });
    }
}

void implementation::universal::input::on_key_down(const event& ev) {
    // Calculate values from prior event
    time_t delta = time(NULL) - pointers[ev.identifier].pressed;

    // Update to current
    keys[ev.identifier].pressed = time(NULL);

    // Track the event
    active_keys.push_back(&keys[ev.identifier]);

    raise({ KEY, DOWN, ev.identifier, { 0.0f, 0.0f, 0.0f }, delta });
}

void implementation::universal::input::on_key_up(const event& ev) {
    auto end = std::remove_if(active_keys.begin(),
                              active_keys.end(),
                              [ev](key const* k) {
                                return k->code == ev.identifier;
                              });
    active_keys.erase(end, active_keys.end());
    raise({ KEY, UP, ev.identifier, { 0.0f, 0.0f, 0.0f }, time(NULL) - keys[ev.identifier].pressed });
}

void implementation::universal::input::emit_active() {
    for (auto active : active_pointers) {
        time_t delta = time(NULL) - active->pressed;
        raise({ POINTER, HELD_DOWN, active->code, active->point, delta });
    }
    for (auto active : active_keys) {
        time_t delta = time(NULL) - active->pressed;
        raise({ KEY, HELD_DOWN, active->code, { 0.0f, 0.0f, 0.0f }, delta });
    }
}

#endif
