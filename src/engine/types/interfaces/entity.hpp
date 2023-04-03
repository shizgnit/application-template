/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
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

#pragma once

namespace type {

    class entity : virtual public type::info, public properties {
    public:

        enum states {
            SELECTED = 0x01,
            GROUPED = 0x02,
            VIRTUAL = 0X04
        };

        typedef long instance_t;
        typedef std::string group_t;

        typedef int key_t;

        operator bool() {
            return instances.size();
        }

        static auto & crossreference() {
            static std::map<instance_t, type::entity*> _crossreference;
            return _crossreference;
        }

        static entity& find(const value_t& criteria) {
            static type::entity empty;
            auto i = crossreference().find(std::get<instance_t>(criteria));
            if (i == crossreference().end()) {
                return empty;
            }
            return *(i->second);
        }

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
            typedef std::function<void ()> callback_t;

            waypoint() {}
            waypoint(const spatial::position& p1, const spatial::position& p2) {
                set(p1, p2);
            }

            waypoint& checkpoint() {
                return checkpoint(start.eye);
            }

            waypoint& checkpoint(const spatial::position& p1) {
                distance = p1.eye.distance(finish.eye);
                return set(p1, finish.eye);
            }

            waypoint& set(const spatial::position& p1, const spatial::position& p2) {
                start = p1;
                finish = p2;
                distance = start.eye.distance(finish.eye);
                finished = false;
                return *this;
            }

            waypoint& speed(double r) {
                rate = r;
                return *this;
            }

            waypoint& orient(const spatial::vector& o) {
                orientation = o;
                return *this;
            }

            waypoint& rotate(const spatial::vector& axis, const spatial::vector& rads) {
                this->axis = axis;
                rotation = rads;
                rotating = true;
                return *this;
            }
            waypoint& on_finish(callback_t c) {
                on_finish_callback = c;
                return *this;
            }

            waypoint& go() {
                active = true;
                delta = distance / rate;
                reference = std::chrono::system_clock::now().time_since_epoch();
                return *this;
            }

            spatial::position position() {
                if (active == false) {
                    return start;
                }

                auto now = std::chrono::system_clock::now().time_since_epoch();
                auto elapsed = std::chrono::duration_cast<utilities::seconds_t>(now - reference).count();
                auto time = elapsed / delta;

                if (time >= 1.0) {
                    active = false;
                    current = finish;
                    finished = true;
                }
                else {
                    current.reposition(start.eye.lerp(finish.eye, time));
                    if (rotating == false) {
                        current.lookat(start.focus.lerp(finish.focus + orientation, time));
                    }
                }
                if (finished && on_finish_callback) {
                    on_finish_callback();
                }

                if (rotating) {
                    current.roll(rotation.z);
                    current.pitch(rotation.x);
                }

                return current;
            }

            utilities::seconds_t reference;
            double rate = 1.0; // relative units per second
            double distance; // total distance between start and finish
            double delta; // travel distance per-second
            double acceleration; // 
            spatial::position start;
            spatial::position finish;
            spatial::position current;

            bool finished = false;
            bool active = false;

            callback_t on_finish_callback;

            bool rotating = false;
            spatial::vector axis;
            spatial::vector rotation;
            spatial::vector orientation;
        };

        class instance : public properties {
        public:
            void define(instance_t i, entity* r, properties& p, size_t idx) {
                id = i;
                parent = r;
                (properties&)*this = p;
                index = idx;
            }

            operator spatial::matrix () {
                return position.serialize();
            }

            instance_t id = 0;

            size_t index = 0;
            bool assigned = false;
            bool hashed = false;

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
            void* bucket = NULL;

            void toggle(entity::states state, bool value) {
                if(value) {
                    switch (state) {
                    case(entity::SELECTED):
                        flags |= entity::SELECTED;
                    }
                }
                else {
                    switch (state) {
                    case(entity::SELECTED):
                        flags &= ~entity::SELECTED;
                    }
                }
                update();
            }

            void update() {
                parent->identifiers.content[index] = id;
                parent->flags.content[index] = flags;
                parent->positions.content[index] = position.serialize();
                if (hashed == false) {
                    parent->store(position.eye, *this);
                }
            }
        };

        typedef std::vector<instance*> bucket_t;

        std::map<key_t, std::map<key_t, std::map<key_t, bucket_t>>> _hash;

        void store(const spatial::vector& p, instance& i) {
            auto& bucket = _hash[p.x / 20][p.y / 20][p.z / 20];
            if (i.bucket) {
                if (i.bucket == &bucket) {
                    return;
                }
                bucket_t* ref = (bucket_t*)i.bucket;
                ref->erase(std::find(ref->begin(), ref->end(), &i));
            }
            bucket.push_back(&i);
            i.bucket = &bucket;
        }

        std::vector<bucket_t*> list(const spatial::vector& p1, const spatial::vector& p2) {
            auto x1 = (p1.x < p2.x ? p1.x : p2.x) / 20;
            auto x2 = (p1.x < p2.x ? p2.x : p1.x) / 20;
            auto y1 = (p1.y < p2.y ? p1.y : p2.y) / 20;
            auto y2 = (p1.y < p2.y ? p2.y : p1.y) / 20;
            auto z1 = (p1.z < p2.z ? p1.z : p2.z) / 20;
            auto z2 = (p1.z < p2.z ? p2.z : p1.z) / 20;
            
            std::vector <bucket_t*> results;

            auto& h = _hash;
            for (int x = x1; x <= x2; x++) {
                auto mx = h.find(x);
                if (mx == h.end()) {
                    continue;
                }
                for (int y = y1; y <= y2; y++) {
                    auto my = mx->second.find(y);
                    if (my == mx->second.end()) {
                        continue;
                    }
                    for (int z = z1; z <= z2; z++) {
                        auto mz = my->second.find(z);
                        if (mz == my->second.end()) {
                            continue;
                        }
                        results.push_back(&mz->second);
                    }
                }
            }

            return results;
        }

        std::list<std::pair<instance_t, size_t>> available;

        std::map<instance_t, instance> instances;

        // http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
        struct {
            std::vector<unsigned int> content;
            type::info::opaque_t *resource = nullptr;
        } identifiers;

        struct {
            std::vector<unsigned int> content;
            type::info::opaque_t *resource = nullptr;
        } flags;

        struct {
            std::vector<spatial::matrix> content;
            type::info::opaque_t *resource = nullptr;
        } positions;
        
        bool grouped = false;

        size_t size = 0;
        size_t capacity = 0;
        size_t limit = 256;
        
        type::info::opaque_t *resource = nullptr;

        bool compile(spatial::position* reference) {
            if (reference) {
                for (auto& sector : list(reference->eye + spatial::vector({ 20, 0, 60 }), reference->eye - spatial::vector({ 20, reference->eye.y, 20 }))) {
                    for (auto entry : *sector) {
                        entry->update();
                    }
                }
            }
            else {
                for (auto& entry : instances) {
                    entry.second.update();
                }
            }
            return compiled() == false;
        }

        instance & add(properties& props=properties::instance(), int count = 1) {
            return getInstance(allocate(props, count));
        }

        instance_t allocate(properties& props, int count) {
            static instance_t increment = 0;
            if (capacity == 0) {
                capacity = limit;
                identifiers.content.resize(capacity);
                flags.content.resize(capacity);
                positions.content.resize(capacity);
            }
            for (int i = 0; i < count; i++) {
                int index = instances.size() >= limit ? (instances.size() % (limit - 1)) + 1 : instances.size() % limit;
                instances[++increment].define(increment, this, props, index);
                crossreference().insert({ increment, this });
            }
            size = instances.size() > (limit) ? (limit) : instances.size();
            return increment;
        }

        void release(instance_t id) {
            if (instances.find(id) == instances.end()) {
                return;
            }
            available.push_back({ id, instances[id].index });
            instances.erase(id);
        }

        bool play(std::string animation, instance_t id=0) {
            if (instances.size() == 0) {
                add();
            }

            instance_t key = (id == 0) ? instances.begin()->first : id;
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

        void animate(instance_t id = 0) {
            if (instances.size() == 0) {
                return;
            }
            if (flag("animated") == false) {
                return;
            }

            instance_t key = (id == 0) ? instances.begin()->first : id;
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
                    instance.second.position.up = position.up;
                    if (instance.second.path.begin()->finished) {
                        instance.second.path.pop_front();
                    }
                    else {
                        break; // the current node isn't finished
                    }
                }
                // Tell the entity compile the object needs to report state updates
                instance.second.assigned = false;
            }
        }

        bool hasInstance(instance_t id) {
            return instances.find(id) != instances.end();
        }

        instance& getInstance(instance_t id = 0) {
            static type::entity::instance empty;

            if (instances.size() == 0) {
                add();
            }
            instance_t key = (id == 0) ? instances.begin()->first : id;
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

        instance_t last = 0;
    };

}
