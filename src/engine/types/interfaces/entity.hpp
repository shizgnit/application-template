#pragma once

namespace type {

    class entity : virtual public type::info {
    public:
        typedef unsigned int key_t;

        std::map<std::string, bool> properties;

        class animation {
        public:
            double elapse = 2.0f; // TODO: this needs to come from the input file
            std::vector<utilities::seconds_t> duration;
            std::vector<type::object> frames;
        };
        std::map<std::string, animation> animations;

        class instance {
        public:
            operator spatial::matrix () {
                return position.scale(scale);
            }

            unsigned int id = 0;
            unsigned int flags = 0;
            int frame = 0;
            utilities::seconds_t elapsed;
            std::string state;
            spatial::position position;
            spatial::vector offset;
            float scale = 1.0;
            std::list<spatial::position> path;

            spatial::vector::type_t distance;
            bool operator<(const instance& that) const {
                return this->distance > that.distance;
            }

            std::map<std::string, bool> properties;

            type::entity* parent = NULL;
        };

        std::list<key_t> available;

        std::map<key_t, instance> instances;

        class director : public spatial::position {

        };
        std::list<director> checkpoints;

        // http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
        struct {
            unsigned int context = 0;
            std::vector<unsigned int> content;
        } identifiers;

        struct {
            unsigned int context = 0;
            std::vector<unsigned int> content;
        } flags;

        struct {
            unsigned int context = 0;
            std::vector<spatial::matrix> content;
        } positions;

        void bake() {
            identifiers.content.clear();
            identifiers.content.reserve(instances.size());
            flags.content.clear();
            flags.content.reserve(instances.size());
            positions.content.clear();
            positions.content.reserve(instances.size());
            //std::sort(instances.begin(), instances.end());
            for (auto& entry : instances) {
                identifiers.content.push_back(entry.second.id);
                flags.content.push_back(entry.second.flags);
                positions.content.push_back(entry.second.position.scale(entry.second.scale));
            }
        }

        instance & add(int count=1) {
            allocate(instances.size() + count);
            return get(last);
        }

        bool allocate(int count) {
            static unsigned int increment = 0;
            int current = available.size() + instances.size();
            for (int i = 0; i < count - current; i++) {
                available.push_back(++increment);
            }
            while (instances.size() < count) {
                last = *available.begin();
                instances[last].properties = properties;
                instances[last].id = last;
                instances[last].parent = this;
                available.pop_front();
            }
            return true;
        }

        void release(key_t id) {
            instances.erase(id);
            available.push_back(id);
        }

        bool play(std::string animation, key_t id=0) {
            //std::lock_guard<std::mutex> scoped(lock);
            if (instances.size() == 0) {
                return false;
            }

            key_t key = id == 0 ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return false;
            }

            instances[key].elapsed = std::chrono::system_clock::now().time_since_epoch();

            if (int frames = animations[animation].frames.size()) {
                if (animations[animation].duration.size() == 0) {
                    utilities::seconds_t duration = utilities::seconds_t{ animations[animation].elapse / (double)frames };
                    animations[animation].duration.resize(frames);
                    for (int i = 0; i < frames; i++) {
                        animations[animation].duration[i] = duration;
                        animations[animation].frames[i].emitter = this;
                    }
                }
                instances[key].state = animation;
                return true;
            }

            return false;
        }

        void animate(key_t id = 0) {
            if (instances.size() == 0) {
                return;
            }

            key_t key = id == 0 ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return;
            }
            if (instances[key].state.empty()) {
                return;
            }

            utilities::seconds_t now = std::chrono::system_clock::now().time_since_epoch();

            int current = instances[key].frame;
            int step = 0;

            if (animations[instances[key].state].duration.size()) {
                while ((instances[key].elapsed + animations[instances[key].state].duration[current + step]) < now) {
                    instances[key].elapsed += animations[instances[key].state].duration[current + step];
                    step += 1;
                    if ((current + step) >= animations[instances[key].state].frames.size()) {
                        current = 0;
                        step = 0;
                    }
                }
            }

            instances[key].frame = current + step;
        }

        bool has(key_t id) {
            return instances.find(id) != instances.end();
        }

        instance& get(key_t id = 0) {
            //std::lock_guard<std::mutex> scoped(lock);
            static type::entity::instance empty;

            if (instances.size() == 0) {
                return empty;
            }

            key_t key = id == 0 ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return empty;
            }
            return instances[key];
        }

        operator type::object& () {
            static type::object empty;

            if (instances.size() == 0) {
                return empty;
            }

            auto &instance = get();
            if (instance.state.empty()) {
                if (play("static", instance.id) == false) {
                    return empty;
                }
            }
            if (animations[instance.state].frames.size() == 0) {
                return empty;
            }

            return animations[instance.state].frames[instance.frame];
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

    protected:

        key_t last = 0;
    };

}
