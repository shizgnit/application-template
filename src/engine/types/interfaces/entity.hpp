#pragma once

namespace type {

    class entity : virtual public type::info, public properties {
    public:
        enum {
            SELECTED = 0x01,
            GROUPED = 0x02
        };

        typedef unsigned int key_t;

        class group : public properties {
        public:
            group() {}

            void add(key_t id) {
                members.push_back(id);
            }

            void remove(key_t id) {
                members.remove(id);
            }

            std::string id;
            std::list<key_t> members;
        };

        class catalog {
        protected:
            std::map<key_t, entity*> instances;
            std::map<std::string, group> groups;

        public:
            catalog() {}

            static catalog& singleton() {
                static catalog instance;
                return instance;
            }

            void add(key_t id, entity* reference, properties& props = properties::empty()) {
                instances[id] = reference;
                if (props.has("grouping")) {
                    auto grouping = std::get<std::string>(props.get("grouping"));
                    if (groups.find(grouping) == groups.end()) {
                        groups[grouping].id = grouping;
                    }
                    groups[grouping].add(id);
                }
            }

            void remove(key_t id, properties& props = properties::empty()) {
                instances.erase(id);
                if (props.has("grouping")) {
                    auto grouping = std::get<std::string>(props.get("grouping"));
                    if (groups.find(grouping) != groups.end()) {
                        groups[grouping].remove(id);
                    }
                }
            }

            void remove(entity& parent) {
                for(auto &child: parent.instances) {
                    remove(child.second.id, child.second);
                }
            }

            bool hasEntity(key_t id) {
                return instances.find(id) != instances.end();
            }

            entity& getEntity(key_t id) {
                static entity empty;
                if (instances.find(id) == instances.end()) {
                    return empty;
                }
                return *instances[id];
            }

            bool hasGroup(const std::string& grouping) {
                return groups.find(grouping) != groups.end();
            }

            group& getGroup(const std::string& grouping) {
                static group empty;
                if (grouping.empty()) {
                    return empty;
                }
                if (groups.find(grouping) == groups.end()) {
                    groups[grouping].id = grouping;
                }
                return groups[grouping];
            }

            std::vector<std::string> groupings() {
                std::vector<std::string> results;
                for (auto& instance : groups) {
                    results.push_back(instance.second.id);
                }
                return results;
            }
        };

        type::object* object = NULL;
        class animation {
        public:
            double elapse = 2.0f; // TODO: this needs to come from the input file
            std::vector<utilities::seconds_t> duration;
            std::vector<type::object> frames;
        };
        std::map<std::string, animation> animations;

        class waypoint {
        public:
            waypoint() {}
            waypoint(const spatial::position& p1, const spatial::position& p2, double r) {
                set(p1, p2, r);
            }

            waypoint& checkpoint() {
                return checkpoint(start.eye);
            }

            waypoint& checkpoint(const spatial::position& p1) {
                distance = p1.eye.distance(finish.eye);
                return set(p1, finish.eye, rate);
            }

            waypoint& set(const spatial::position& p1, const spatial::position& p2, double r) {
                rate = r;
                start = p1;
                finish = p2;
                distance = p1.eye.distance(p2.eye);
                reference = std::chrono::system_clock::now().time_since_epoch();
                finished = false;
                return *this;
            }

            waypoint& go() {
                active = true;
                return *this;
            }

            spatial::position position() {
                if (active == false) {
                    return start;
                }

                duration = distance / rate;
                auto now = std::chrono::system_clock::now().time_since_epoch();
                auto diff = std::chrono::duration_cast<utilities::seconds_t>(now - reference).count();
                auto travel = diff / duration;

                if (travel >= 1.0) {
                    active = false;
                    finished = true;
                    current = finish;
                }
                else {
                    current.reposition(start.eye.lerp(finish.eye, travel));
                    current.lookat(start.focus.lerp(finish.focus, travel));
                }

                return current;
            }

            utilities::seconds_t reference = std::chrono::system_clock::now().time_since_epoch();
            double rate = 1.0; // units per second
            double distance; // units
            double duration;
            spatial::position start;
            spatial::position finish;
            spatial::position current;

            bool finished = false;
            bool active = false;
        };

        class instance : public properties {
        public:
            void define(key_t i, entity* r, properties& p) {
                id = i;
                parent = r;
                (properties&)*this = p;
            }

            operator spatial::matrix () {
                return position.scale();
            }

            key_t id = 0;

            unsigned int flags = 0;
            int frame = 0;
            utilities::seconds_t elapsed;
            std::string state;
            spatial::position position;
            spatial::vector offset;

            std::list<waypoint> path;

            spatial::vector::type_t distance;
            bool operator<(const instance& that) const {
                return this->distance > that.distance;
            }

            type::entity* parent = NULL;
        };

        std::list<key_t> available;

        std::map<key_t, instance> instances;

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

        bool grouped = false;

        int baked = 0;
        void bake() {
            identifiers.content.clear();
            identifiers.content.reserve(baked);
            flags.content.clear();
            flags.content.reserve(baked);
            positions.content.clear();
            positions.content.reserve(baked);
            for (auto& entry : instances) {
                if (entry.second.has("grouping")) {
                    entry.second.flags |= type::entity::GROUPED;
                }
                else {
                    entry.second.flags &= UINT_MAX ^ type::entity::GROUPED;
                }
                identifiers.content.push_back(entry.second.id);
                flags.content.push_back(entry.second.flags);
                positions.content.push_back(entry.second.position.scale());
            }
            baked = instances.size();
        }

        instance & add(properties& props=properties::empty(), int count = 1) {
            allocate(props, instances.size() + count);
            return getInstance(last);
        }

        bool allocate(properties& props, int count) {
            static unsigned int increment = 0;
            int current = available.size() + instances.size();
            for (int i = 0; i < count - current; i++) {
                available.push_back(++increment);
            }
            while (instances.size() < count) {
                last = *available.begin();
                instances[last].define(last, this, props);
                catalog::singleton().add(last, this, props);
                available.pop_front();
            }
            return true;
        }

        void release(key_t id) {
            instances.erase(id);
            available.push_back(id);
            catalog::singleton().remove(id); // TODO: this should include the group the id belongs to
        }

        bool play(std::string animation, key_t id=0) {
            //std::lock_guard<std::mutex> scoped(lock);
            if (instances.size() == 0) {
                return false;
            }

            key_t key = (id == 0) ? instances.begin()->first : id;
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
            if (flag("animated") == false) {
                return;
            }

            key_t key = (id == 0) ? instances.begin()->first : id;
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

            for (auto& instance : instances) {
                while(instance.second.path.size()) {
                    auto position = instance.second.path.begin()->position();
                    instance.second.position.reposition(position.eye);
                    instance.second.position.lookat(position.focus);
                    if (instance.second.path.begin()->finished) {
                        instance.second.path.pop_front();
                    }
                    else {
                        break; // the current node isn't finished
                    }
                }
            }

        }

        bool hasInstance(key_t id) {
            return instances.find(id) != instances.end();
        }

        instance& getInstance(key_t id = 0) {
            //std::lock_guard<std::mutex> scoped(lock);
            static type::entity::instance empty;

            if (instances.size() == 0) {
                return empty;
            }
            key_t key = (id == 0) ? instances.begin()->first : id;
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

            auto &instance = getInstance();
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
