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
        };

        std::list<unsigned int> available;

        std::map<unsigned int, instance> instances;

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
            for (auto entry : instances) {
                identifiers.content.push_back(entry.second.id);
                flags.content.push_back(entry.second.flags);
                positions.content.push_back(entry.second.position);
            }
        }

        bool allocate(int count) {
            static unsigned int increment = 0;
            int current = available.size() + instances.size();
            for (int i = 0; i < count - current; i++) {
                available.push_back(++increment);
            }
            while (instances.size() < count) {
                instances[*available.begin()].id = *available.begin();
                available.pop_front();
            }
            return true;
        }

        void release(int id) {
            instances.erase(id);
            available.push_back(id);
        }

        void play(std::string animation, int id=0) {
            //std::lock_guard<std::mutex> scoped(lock);
            allocate(1);

            int key = id == 0 ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return;
            }

            instances[key].elapsed = std::chrono::system_clock::now().time_since_epoch();
            if (animations[animation].duration.size() == 0) {
                int frames = animations[animation].frames.size();
                utilities::seconds_t duration = utilities::seconds_t{ animations[animation].elapse / (double)frames };
                animations[animation].duration.resize(frames);
                for (int i = 0; i < frames; i++) {
                    animations[animation].duration[i] = duration;
                    animations[animation].frames[i].emitter = this;
                }
            }
            instances[key].state = animation;
        }

        void animate(int id = 0) {
            allocate(1);

            int key = id == 0 ? instances.begin()->first : id;
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

        instance& get(int id = 0) {
            //std::lock_guard<std::mutex> scoped(lock);
            static type::entity::instance empty;

            allocate(1);

            int key = id == 0 ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return empty;
            }
            return instances[key];
        }

        operator type::object& () {
            static type::object empty;

            auto instance = get();
            if (instance.state.empty()) {
                return empty;
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

        bool camera = false;
    };

}
