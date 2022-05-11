#pragma once

namespace type {

    class entity : virtual public type::info {
    public:
        class animation {
        public:
            double elapse = 2.0f; // TODO: this needs to come from the input file
            std::vector<utilities::seconds_t> duration;
            std::vector<type::object> frames;
        };
        std::map<std::string, animation> animations;

        class instance {
        public:
            instance() {
                static int increment = 0;
                id = ++increment;
            }
            int id = 0;
            int frame = 0;
            utilities::seconds_t elapsed;
            std::string state;
            spatial::position position;
            std::list<spatial::position> path;

            spatial::vector::type_t distance;
            bool operator<(const instance& that) const {
                return this->distance > that.distance;
            }
        };
        std::vector<instance> instances;

        class director : public spatial::position {

        };
        std::list<director> checkpoints;

        unsigned int context = 0;

        unsigned int flags = 0;

        // http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
        std::vector<spatial::matrix> positions;

        void position() {
            positions.clear();
            //std::sort(instances.begin(), instances.end());
            for (auto entry : instances) {
                positions.push_back(entry.position);
            }
        }

        void allocate(int count) {
            if (instances.size() < count) {
                instances.resize(count);
            }
        }

        void play(std::string animation, int index=0) {
            //std::lock_guard<std::mutex> scoped(lock);
            allocate(index + 1);
            instances[index].elapsed = std::chrono::system_clock::now().time_since_epoch();
            if (animations[animation].duration.size() == 0) {
                int frames = animations[animation].frames.size();
                utilities::seconds_t duration = utilities::seconds_t{ animations[animation].elapse / (double)frames };
                animations[animation].duration.resize(frames);
                for (int i = 0; i < frames; i++) {
                    animations[animation].duration[i] = duration;
                    animations[animation].frames[i].emitter = this;
                }
            }
            instances[index].state = animation;
        }

        void animate(int index = 0) {
            allocate(index + 1);
            if (instances[index].state.empty()) {
                return;
            }

            utilities::seconds_t now = std::chrono::system_clock::now().time_since_epoch();

            int current = instances[index].frame;
            int step = 0;

            while ((instances[index].elapsed + animations[instances[index].state].duration[current + step]) < now) {
                instances[index].elapsed += animations[instances[index].state].duration[current + step];
                step += 1;
                if ((current + step) >= animations[instances[index].state].frames.size()) {
                    current = 0;
                    step = 0;
                }
            }

            instances[index].frame = current + step;
        }

        type::object& get(int index = 0) {
            //std::lock_guard<std::mutex> scoped(lock);
            allocate(index + 1);
            static type::object empty;
            if (instances[index].state.empty()) {
                return empty;
            }
            return animations[instances[index].state].frames[instances[index].frame];
        }

        operator type::object& () {
            return get(0);
        }

        platform::input events;

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
