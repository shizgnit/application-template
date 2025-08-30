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

        // 1111 1100 0000 0000 0000 0000 0000 0000

        enum states {
            SELECTED = 0x00000001,
            GROUPED  = 0x00000002,
            VIRTUAL  = 0X00000004,
            ALPHA    = 0x00000008,
            UV       = 0x00000010,
            OFFSET   = 0x00000020,
            ACTIVE   = 0x00000040
        };

        typedef long instance_t;
        typedef std::string group_t;

        typedef int key_t;

        static auto& reference() {
            static std::unordered_map<instance_t, type::entity*> _reference;
            return _reference;
        }

        static entity& find(value_t criteria) {
            static type::entity empty;
            auto i = reference().find(std::get<instance_t>(criteria));
            if (i == reference().end() || i->second == nullptr) {
                return empty;
            }
            return *(i->second);
        }

        entity() {
            int x = 1;
        }

        std::string type() {
            return "type::entity";
        }

        class instance : public properties {
        public:
            instance() {}
            instance(instance_t i, type::entity* r, properties& p, size_t idx) {
                id = i;
                entity = r;
                (properties&)*this = p;
                index = idx;
                reference().insert({ i, r });
            }

            void toggle(entity::states state, bool value) {
                if(value) {
                    switch (state) {
                    case(entity::SELECTED):
                        flags |= entity::SELECTED;
                    };
                }
                else {
                    switch (state) {
                    case(entity::SELECTED):
                        flags &= ~entity::SELECTED;
                    };
                }
                dirty = true;
            }

            void update() {
                dirty = true;
            }

            operator spatial::matrix () {
                return position.serialize();
            }

            bool operator<(const instance& that) const {
                return this->distance > that.distance;
            }

            bool active = true;
            bool dirty = true;

            instance_t id = 0;
            size_t index = 0;

            unsigned int flags = 0;
            spatial::position position;
            unsigned int frame = 0;
            std::list<type::waypoint> path;
            std::pair<unsigned int, unsigned int> offset;

            spatial::vector::type_t distance;
            type::entity* entity = NULL;
            type::rig::bone *rigging = NULL;
            std::pair<int, int> quadrant;
        };

        struct {
            std::vector<unsigned int> content;
            type::info::opaque_t *resource = nullptr;
        } offsets;

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

        instance& addInstance(properties& props=properties::instance(), int count = 1) {
            static instance_t id = 0;
            if (available.size() == 0 && instances.size() == 0) {
                offsets.content.resize(capacity*2);
                identifiers.content.resize(capacity);
                flags.content.resize(capacity);
                positions.content.resize(capacity);
                for (int i = 0; i < capacity; i++) {
                    available.push_back({ ++id, std::make_shared<instance>(id, this, props, i) });
                }
            }
            instance_t last = 0;
            for(int i = 0; i < count && available.size(); i++) {
                auto instance = available.front();
                instances.insert({ instance.first, instance.second });
                last = instance.first;
                available.pop_front();
            }
            return getInstance(last);
        }

        bool hasInstance(instance_t id) {
            return instances.find(id) != instances.end();
        }

        instance& getInstance(instance_t id = 0) {
            static type::entity::instance empty;
            if (instances.size() == 0) {
                addInstance();
            }
            instance_t key = (id == 0) ? instances.begin()->first : id;
            if (instances.find(key) == instances.end()) {
                return empty;
            }
            return *instances[key].get();
        }

         std::unordered_map<instance_t, std::shared_ptr<instance>>& getInstances() {
            return instances;
        }

        void releaseInstance(instance_t id, bool free=false) {
            auto instance = instances.find(id);
            if (instance == instances.end()) {
                return;
            }
            if(free) {
                reference().erase(id);
                instances.erase(id);
            }
            else {
                available.push_back({ id, instance->second });
            }
        }

        void setObject(std::shared_ptr<type::object> object) {
            this->object = object;
            this->object->emitter = this;
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

            utilities::seconds_t now = std::chrono::system_clock::now().time_since_epoch();

            std::vector<instance_t> cleanup;
            for (auto& instance : instances) {
                while(instance.second->path.size()) {
                    auto position = instance.second->path.begin()->get();
                    if (instance.second->rigging) {
                        instance.second->rigging->adjust(position, position.eye);
                    } 
                    else {
                        instance.second->position.reposition(position.eye);
                        if (position.translation.active) {
                            instance.second->position.orientation(position);
                        }
                        else {
                            instance.second->position.lookat(position.focus);
                        }
                        instance.second->position.alpha = position.alpha;
                        instance.second->flags &= 0xFF;
                        instance.second->flags |= (unsigned int)(255.0 * position.alpha) << 24;
                        instance.second->update();
                    }
                    if (instance.second->path.begin()->finished) {
                        if (instance.second->path.begin()->terminator) {
                            cleanup.push_back(instance.second->id);
                        }
                        instance.second->path.pop_front();
                    }
                    else {
                        break; // the current node isn't finished
                    }
                }
            }
            for (auto id : cleanup) {
                releaseInstance(id);
            }
        }

        bool compile(spatial::position* reference) {
           for (auto& instance : instances) {
                if (instance.second->dirty == false) {
                    continue;
                }
                offsets.content[(instance.second->index * 2)] = instance.second->offset.first;
                offsets.content[(instance.second->index * 2) + 1] = instance.second->offset.second;
                identifiers.content[instance.second->index] = instance.second->id;
                flags.content[instance.second->index] = instance.second->flags;
                positions.content[instance.second->index] = instance.second->position.serialize();
                instance.second->dirty = false;
            }
            return compiled() == false;
        }

        bool empty() {
            return instances.size() == 0;
        }

        size_t size() {
            return instances.size();
        }

        operator bool() {
            return instances.size();
        }

        type::info::opaque_t *resource = nullptr;
        std::shared_ptr<type::object> object;
        operator type::object& () {
            return *object.get();
        }

        std::unordered_map<instance_t, std::shared_ptr<instance>> instances;

    protected:
        const size_t capacity = 256;
        std::list<std::pair<instance_t, std::shared_ptr<instance>>> available;

        bool grouped = false;
        
        platform::input events;

        int sector_size = 20;
        spatial::vector bound_bottom_left = { 20, 0, 20 };
        spatial::vector bound_top_right = { 20, 0, 20 };
    };

}

