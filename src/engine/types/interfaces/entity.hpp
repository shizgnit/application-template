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

        enum {
            SELECTED = 0x01,
            GROUPED = 0x02,
            VIRTUAL = 0X04
        };

        typedef long instance_t;
        typedef std::string group_t;

        operator bool() {
            return instances.size();
        }

        static auto & cache() {
            static std::map<instance_t, type::entity*> _cache;
            return _cache;
        }

        static entity& find(const value_t& criteria) {
            static type::entity empty;
            auto i = cache().find(std::get<instance_t>(criteria));
            if (i == cache().end()) {
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

            waypoint& set(const spatial::position& p1, const spatial::position& p2, double r, double a=1.0) {
                rate = r;
                start = p1;
                finish = p2;
                distance = start.eye.distance(finish.eye);
                speed = distance / rate;
                finished = false;
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
                reference = std::chrono::system_clock::now().time_since_epoch();
                return *this;
            }

            spatial::position position() {
                if (active == false) {
                    return start;
                }

                auto now = std::chrono::system_clock::now().time_since_epoch();
                auto delta = std::chrono::duration_cast<utilities::seconds_t>(now - reference).count();
                auto time = delta / speed;

                if (time >= 1.0) {
                    active = false;
                    current = finish;
                    finished = true;
                }
                else {
                    current.reposition(start.eye.lerp(finish.eye, time));
                    if (rotating == false) {
                        current.lookat(start.focus.lerp(finish.focus, time));
                    }
                }
                if (finished && on_finish_callback) {
                    on_finish_callback();
                }

                if (rotating) {
                    //current.eye = spatial::matrix().rotate_x(rotation.x).interpolate(current.eye);
                    //current.eye = spatial::matrix().rotate_y(rotation.y).interpolate(current.eye);
                    current.roll(rotation.z); // 90.0f * (delta > 1.0 ? 1.0 : delta));
                    current.pitch(rotation.x); // 90.0f * (delta > 1.0 ? 1.0 : delta));
                    //current.reposition(spatial::matrix().rotate_z(rotation.z * (delta / (rotation.z / rate))).interpolate(current.eye - axis) + axis);
                }

                return current;
            }

            utilities::seconds_t reference;
            double rate = 1.0; // relative units per second
            double distance; // total distance between start and finish
            double speed; // travel distance per-second
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

        typedef std::function<bool(type::entity::instance &)> callback_t;
        callback_t culling_criteria;
        void setCullCriteria(callback_t callback) {
           culling_criteria = callback;
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
        size_t growth = 48;
        
        type::info::opaque_t *resource = nullptr;

        bool compile() {
            for (auto& entry : instances) {
                if (culling_criteria && culling_criteria(entry.second)) {
                    continue;
                }
                if (entry.second.assigned) {
                    continue;
                }
                identifiers.content[entry.second.index] = entry.second.id;
                flags.content[entry.second.index] = entry.second.flags;
                positions.content[entry.second.index] = entry.second.position.serialize();
                entry.second.assigned = true;
            }
            return compiled() == false;
        }

        instance & add(properties& props=properties::instance(), int count = 1) {
            allocate(props, count);
            size = instances.size();
            return getInstance(last);
        }

        bool allocate(properties& props, int count) {
            static instance_t increment = 0;
            if (count > available.size()) {
				size_t size = available.size() + instances.size();
				size_t needed = count - available.size();
				size_t allocation = 0;
				while((size + needed) > capacity) {
					allocation += growth;
					capacity += growth;
				}
				if (allocation) {
					identifiers.content.resize(capacity);
					flags.content.resize(capacity);
					positions.content.resize(capacity);
                    size_t offset = capacity - growth;
                    memset(identifiers.content.data() + offset, 0, growth * sizeof(unsigned int));
                    memset(flags.content.data() + offset, 0, growth * sizeof(unsigned int));
                    memset(positions.content.data() + offset, 0, growth * sizeof(spatial::matrix));
                    for (int i = 0; i < allocation; i++) {
                        available.push_back({ ++increment, capacity - allocation + i });
                    }
                    compiled(false);
                }
            }
            while (count) {
                last = available.begin()->first;
                instances[last].define(last, this, props, available.begin()->second);
                cache().insert({ last, this });
                available.pop_front();
                count -= 1;
            }
            return true;
        }

        void release(instance_t id) {
            if (instances.find(id) == instances.end()) {
                return;
            }
            available.push_back({ id, instances[id].index });
            instances.erase(id);
        }

        bool play(std::string animation, instance_t id=0) {
            //std::lock_guard<std::mutex> scoped(lock);
            if (instances.size() == 0) {
                //return false;
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
            //std::lock_guard<std::mutex> scoped(lock);
            static type::entity::instance empty;

            if (instances.size() == 0) {
                //return empty;
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
