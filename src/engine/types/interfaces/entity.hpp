/*
================================================================================
  Copyright (c) 2023, Pandemos
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

        // 1110 0000 0000 0000 0000 0001 0000 0000

        enum states {
            SELECTED = 0x00000001,
            GROUPED  = 0x00000002,
            VIRTUAL  = 0X00000004,
            ALPHA    = 0x00000008 
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

        static entity& find(value_t criteria) {
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
            typedef std::function<bool (const spatial::position& p)> terminate_t;

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
                rotation.finish = {translation.translation.pitch, translation.translation.spin, translation.translation.roll};
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
            waypoint& g(float a, float v, float g=9.8) {
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

                if(terminate_callback && terminate_callback(position.current)) {
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
            type::rig::bone *rigging = NULL;
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
                if (position.alpha == 0.0) {
                    flags |= type::entity::VIRTUAL | type::entity::ALPHA;
                }
                assign();
                store();
            }

            void assign() {
                parent->identifiers.content[index] = id;
                parent->flags.content[index] = flags;
                parent->positions.content[index] = position.serialize();
            }

            void store() {
                parent->store(position.eye, *this);
            }

            void clear() {
                parent->identifiers.content[index] = 0;
                parent->flags.content[index] = 0x00;
                parent->positions.content[index] = spatial::matrix();
            }
        };

        typedef std::list<instance*> bucket_t;

        std::map<key_t, std::map<key_t, std::map<key_t, bucket_t>>> _hash;
        int sector_size = 20;
        spatial::vector bound_bottom_left = { 20, 0, 20 };
        spatial::vector bound_top_right = { 20, 0, 20 };

        void store(const spatial::vector& p, instance& i) {
            auto& bucket = _hash[(key_t)(p.x / sector_size)][(key_t)(p.y / sector_size)][(key_t)(p.z / sector_size)];
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
            key_t x1 = (p1.x < p2.x ? p1.x : p2.x) / sector_size;
            key_t x2 = (p1.x < p2.x ? p2.x : p1.x) / sector_size;
            key_t y1 = (p1.y < p2.y ? p1.y : p2.y) / sector_size;
            key_t y2 = (p1.y < p2.y ? p2.y : p1.y) / sector_size;
            key_t z1 = (p1.z < p2.z ? p1.z : p2.z) / sector_size;
            key_t z2 = (p1.z < p2.z ? p2.z : p1.z) / sector_size;
            
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
                for (auto& sector : list(reference->eye + bound_bottom_left, reference->eye - spatial::vector({ bound_top_right.x, reference->eye.y, bound_top_right.z}))) {
                    for (auto entry : *sector) {
                        entry->assign();
                    }
                }
            }
            else {
                for (auto& entry : instances) {
                    entry.second.assign();
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

        void release(instance_t id, bool free=false) {
            auto instance = instances.find(id);
            if (instance == instances.end()) {
                return;
            }
            if (instance->second.bucket) {
                bucket_t* ref = (bucket_t*)instance->second.bucket;
                ref->erase(std::find(ref->begin(), ref->end(), &instance->second));
            }
            if(free) {
                instance->second.clear();
                crossreference().erase(id);
                instances.erase(id);
            }
            else {
                available.push_back({ id, instance->second.index });
            }
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

            std::vector<instance_t> cleanup;
            for (auto& instance : instances) {
                while(instance.second.path.size()) {
                    auto position = instance.second.path.begin()->get();
                    if (instance.second.rigging) {
                        instance.second.rigging->adjust(position, position.eye);
                    } 
                    else {
                        instance.second.position.reposition(position.eye);
                        if (position.translation.active) {
                            instance.second.position.orientation(position);
                        }
                        else {
                            instance.second.position.lookat(position.focus);
                        }
                        instance.second.position.alpha = position.alpha;
                        instance.second.flags &= 0xFF;
                        instance.second.flags |= (unsigned int)(255.0 * position.alpha) << 24;
                        instance.second.update();
                    }
                    if (instance.second.path.begin()->finished) {
                        if (instance.second.path.begin()->terminator) {
                            cleanup.push_back(instance.second.id);
                        }
                        instance.second.path.pop_front();
                    }
                    else {
                        break; // the current node isn't finished
                    }
                }
            }
            for (auto id : cleanup) {
                release(id);
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
            return instances.size() == 0;
        }

    protected:

        instance_t last = 0;
    };

}
