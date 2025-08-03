#pragma once

namespace type {

    class waypoint {
    public:
        typedef std::function<void()> callback_t;
        typedef std::function<bool(const spatial::position& p)> terminate_t;

        waypoint(const spatial::position& p1) {
            position.start = p1;
            position.current = p1;
            position.finish = p1;
        }
        waypoint(const spatial::position& p1, const spatial::position& p2) {
            p(p1, p2);
        }

        waypoint& checkpoint() {
            return checkpoint(position.start.eye);
        }

        waypoint& checkpoint(const spatial::position& p1) {
            position.distance = p1.eye.distance(position.finish.eye);
            return p(p1, position.finish.eye);
        }

        waypoint& p(const spatial::position& p1, const spatial::position& p2) {
            position.start = p1;
            position.current = p1;
            position.finish = p2;
            position.distance = position.start.eye.distance(position.finish.eye);
            position.active = true;
            finished = false;
            return *this;
        }
        waypoint& r(const spatial::position& translation) {
            rotation.finish = { translation.translation.pitch, translation.translation.spin, translation.translation.roll };
            rotation.active = true;
            finished = false;
            return *this;
        }
        waypoint& r(const spatial::vector& translation, bool continuous = false) {
            rotation.finish = translation;
            rotation.continuous = continuous;
            rotation.active = true;
            finished = false;
            return *this;
        }
        waypoint& a(float start, float finish) {
            alpha.start = start;
            alpha.finish = finish;
            alpha.active = true;
            finished = false;
            return *this;
        }
        waypoint& s(float start, float finish) {
            scale.start = start;
            scale.finish = finish;
            scale.active = true;
            finished = false;
            return *this;
        }
        waypoint& g(float a, float v, float g = 9.8) {
            gravity.arc = position.start;
            gravity.current = position.start;
            gravity.radians = v * (a * M_PI / 180.0); // Includes velocity along the vector
            gravity.gravity = g * 0.5;
            gravity.velocity = v;
            gravity.relative.x = 0.0;
            gravity.relative.y = 0.0;
            gravity.active = true;
            finished = false;
            return *this;
        }

        waypoint& speed(double s) {
            position.speed = s;
            return *this;
        }

        waypoint& orient(const spatial::vector& o) {
            position.orientation = o;
            return *this;
        }

        waypoint& on_finish(callback_t c) {
            on_finish_callback = c;
            return *this;
        }

        waypoint& terminate() {
            terminator = true;
            return *this;
        }

        waypoint& terminate(terminate_t callback) {
            terminator = true;
            terminate_callback = callback;
            return *this;
        }

        waypoint& go() {
            active = true;
            reference = std::chrono::system_clock::now().time_since_epoch();
            if (position.active) {
                duration = position.distance / position.speed;
            }
            else if (duration == 0) {
                duration = 600;
            }
            return *this;
        }

        spatial::position get() {
            if (active == false) {
                return finished ? position.finish : position.start;
            }

            auto now = std::chrono::system_clock::now().time_since_epoch();
            auto time = std::chrono::duration_cast<utilities::seconds_t>(now - reference).count();
            auto elapsed = std::min(time / duration, 1.0);
            auto remaining = 1.0 - elapsed;

            if (terminate_callback && terminate_callback(position.current)) {
                elapsed = 1.0;
            }

            if (elapsed >= 1.0) {
                active = false;
                finished = true;
                if (gravity.active == false) {
                    position.finish.translation = position.current.translation;
                    position.current = position.finish;
                }
                if (scale.active) {
                    position.current.scale(scale.finish);
                }
                if (alpha.active) {
                    position.current.opacity(alpha.finish);
                }
                if (rotation.active) {
                    position.current.translation.active = true;
                    position.current.spin(rotation.continuous ? rotation.finish.y * time : rotation.finish.y - rotation.travel.y, rotation.continuous);
                    position.current.roll(rotation.continuous ? rotation.finish.z * time : rotation.finish.z - rotation.travel.z, rotation.continuous);
                    position.current.pitch(rotation.continuous ? rotation.finish.x * time : rotation.finish.x - rotation.travel.x, rotation.continuous);
                    rotation.travel = rotation.finish;
                }
            }
            else {
                if (gravity.active) {
                    float x = gravity.velocity * time;
                    float y = position.start.eye.y + (gravity.radians * time) - (gravity.gravity * time * time);
                    gravity.arc.surge(x - gravity.relative.x);
                    gravity.arc.heave(y - gravity.relative.y);
                    gravity.relative.x = x;
                    gravity.relative.y = y;
                    position.current.reposition(gravity.arc.eye);
                }
                if (position.active) {
                    position.current.reposition(position.start.eye.lerp(position.finish.eye, elapsed));
                }
                if (rotation.active == false) {
                    position.current.lookat(position.start.focus.lerp(position.finish.focus + position.orientation, elapsed));
                }
                if (scale.active) {
                    position.current.scale(remaining * scale.start + elapsed * scale.finish);
                }
                if (alpha.active) {
                    position.current.opacity(remaining * alpha.start + elapsed * alpha.finish);
                }
                if (rotation.active) {
                    spatial::vector relative = rotation.continuous ? rotation.finish * time : rotation.start.slerp(rotation.finish, elapsed);
                    position.current.translation.active = true;
                    position.current.spin(relative.y - rotation.travel.y, rotation.continuous);
                    position.current.roll(relative.z - rotation.travel.z, rotation.continuous);
                    position.current.pitch(relative.x - rotation.travel.x, rotation.continuous);
                    rotation.travel = relative;
                }
            }

            if (finished && on_finish_callback) {
                on_finish_callback();
            }

            return position.current;
        }

        struct {
            bool active = false;
            float start;
            float finish;
        } alpha;

        struct {
            bool active = false;
            float start;
            float finish;
        } scale;

        struct {
            bool active = false;
            double speed = 1.0; // relative units per second
            double distance; // total distance between start and finish
            double acceleration; // 
            spatial::position start;
            spatial::position finish;
            spatial::position current;
            spatial::vector orientation;
        } position;

        struct {
            bool active = false;
            bool continuous = false;
            spatial::vector finish;
            spatial::vector travel;
            spatial::vector start;
        } rotation;

        struct {
            bool active = false;
            spatial::position start;
            spatial::position arc;
            spatial::position current;
            float radians;
            float velocity;
            float gravity;
            struct {
                float x;
                float y;
            } relative;
        } gravity;

        utilities::seconds_t reference;
        double duration = 0.0; // total travel time

        bool finished = false;
        bool active = false;

        bool terminator = false;

        callback_t on_finish_callback;
        terminate_t terminate_callback;
    };

}
