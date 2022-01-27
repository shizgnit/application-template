#pragma once

namespace type {

    class entity : virtual public type::info {
    public:
        class animation {
        public:
            int frame = 0;
            utilities::time_t start;
            utilities::seconds_t elapsed;

            std::vector<utilities::seconds_t> duration;
            std::vector<type::object> frames;
        };
        std::string state;
        std::map<std::string, animation> animations;

        std::list<spatial::position> path;

        void play(std::string animation, int seconds = 2) {
            //std::lock_guard<std::mutex> scoped(lock);
            animations[animation].start = std::chrono::high_resolution_clock::now();
            animations[animation].elapsed = animations[animation].start.time_since_epoch();
            if (animations[animation].duration.size() == 0) {
                int frames = animations[animation].frames.size();
                utilities::seconds_t duration = utilities::seconds_t{ seconds / (double)frames };
                animations[animation].duration.resize(frames);
                for (int i = 0; i < frames; i++) {
                    animations[animation].duration[i] = duration;
                }
            }
            state = animation;
        }

        void animate() {
            if (state.empty()) {
                return;
            }

            utilities::seconds_t now = std::chrono::high_resolution_clock::now().time_since_epoch();

            int current = animations[state].frame;
            int step = 0;

            while ((animations[state].elapsed + animations[state].duration[current + step]) < now) {
                animations[state].elapsed += animations[state].duration[current + step];
                step += 1;
                if ((current + step) >= animations[state].frames.size()) {
                    current = 0;
                    step = 0;
                }
            }

            animations[state].frame = current + step;
        }

        operator type::object& () {
            //std::lock_guard<std::mutex> scoped(lock);
            static type::object empty;
            if (state.empty()) {
                return empty;
            }
            return animations[state].frames[animations[state].frame];
        }

    public:
        entity() {}

        std::string type() {
            return "type::entity";
        }

        bool empty() {
            return true;
        }
    };

}
