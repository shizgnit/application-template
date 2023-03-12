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

namespace platform {

    class interface {
    public:
        static int next() {
            static int id = 0;
            return id++;
        }

        type::program shader;
        type::font font;
        spatial::matrix projection;

        // Base class for all gui elements
        class widget {
        friend class platform::interface;
        private:
            widget() {} // hide the default constructor

        public:
            enum spec {
                none,
                button,
                textbox,
                tabbed,
                progress,
                panel
            };

            widget(spec selection) {
                this->specification = selection;
            }
            ~widget() {}

            enum positioning {
                absolute = 0x00,
                left     = (1u << 0),
                right    = (1u << 1),
                initial  = (1u << 2),
                inherit  = (1u << 3),
                hcenter  = (1u << 4),
                vcenter  = (1u << 5),
                top      = (1u << 6),
                bottom   = (1u << 7)
            };

            virtual widget& position(int x, int y, positioning relativity=positioning::absolute) {
                this->x = x;
                this->y = y;

                if (relativity & positioning::left) {
                    horizontal = positioning::left;
                }
                if (relativity & positioning::hcenter) {
                    horizontal = positioning::hcenter;
                }
                if (relativity & positioning::right) {
                    horizontal = positioning::right;
                }
                if (relativity & positioning::top) {
                    vertical = positioning::top;
                }
                if (relativity & positioning::vcenter) {
                    vertical = positioning::vcenter;
                }
                if (relativity & positioning::bottom) {
                    vertical = positioning::bottom;
                }

                return *this;
            }

            platform::input events;

            type::object background;
            type::object foreground;
            type::object edge;

            spatial::geometry bounds;

            int x;
            int y;

            std::string label;

            int id;
            spec specification;

            bool selectable = false;
            bool input = false;
            bool enabled = true;
            bool visible = true;

            bool passthrough = false;

            std::vector<widget*> children;

        protected:
            bool floating = false;
            positioning horizontal = positioning::absolute;
            positioning vertical = positioning::absolute;
        };

        class button : public widget {
        public:
            button() : widget(widget::spec::button) { 
                this->id = platform::interface::next();
            }

            std::string label;

            //::type::object icon;
        };

        class textbox : public widget {
        public:
            textbox() : widget(widget::spec::textbox) {
                this->id = platform::interface::next();
            }

            utilities::text content;

            positioning alignment = positioning::top;

            bool multiline = true;
        };

        class progress : public widget {
        public:
            progress() : widget(widget::spec::progress) { 
                this->id = platform::interface::next();
            }

            utilities::percentage value;

            positioning alignment = positioning::hcenter;
        };

        class panel : public widget {
        public:
            panel() : widget(widget::spec::panel) {
                this->id = platform::interface::next();
            }

            int add(interface::widget* child) {
                if (vertical) {
                    auto offset = children.size() ? children.back()->y : 0;
                    child->position(this->x + margin, this->y + offset + margin);
                }
                else {
                    auto offset = children.size() ? children.back()->x : 0;
                    child->position(this->x + offset + margin, this->y + margin);
                }
                children.push_back(child);
                return 0;
            }

            positioning alignment = positioning::right;
            float margin = 1.0;

            bool vertical = false;
        };

        class tabbed : public widget {
        public:
            tabbed() : widget(widget::spec::tabbed) {
                this->id = platform::interface::next();
            }

            int add(interface::widget& button, interface::widget& content) {
                children.push_back(&button);
                crossreference[" "] = children.size();
                children.push_back(&content);
                return 0;
            }

            void select(const std::string label) {
                if (crossreference.find(label) != crossreference.end()) {
                    selected = crossreference[label];
                }
            }

        protected:
            int selected = -1;

            std::map<std::string, int> crossreference;
        };

        virtual bool raise(const input::event& ev, int x, int y) = 0;
        virtual void emit() = 0;

        virtual void position() = 0;
        virtual widget* reposition(std::vector<widget*>& c) = 0;

        virtual void draw() = 0;

        virtual widget* create(std::vector<widget *> &c) = 0;

        virtual widget* create(widget::spec t, int w, int h, int r, int g, int b, int a) = 0;
        virtual widget* create(widget* instance, int w, int h, int r, int g, int b, int a) = 0;

        virtual void print(int x, int y, const std::string& text) = 0;

        template<class t> t& get(int id) {
            // TODO: protect this a bit, check for nulls, etc
            return *dynamic_cast<t*>(instances[id]);
        }

        template<class t> t& cast(widget& instance) {
            return dynamic_cast<t&>(instance);
        }

        virtual bool active() {
            return selected != NULL;
        }

        virtual void select(widget* target) {
            // Unselect the current selected
            if (selected != NULL && (target == NULL || selected != target)) {
                input::event select;
                select.gesture = platform::input::UNSELECT;
                selected->events.raise(select);
                selected = NULL;
            }
            // Track and pass along the events
            if (target) {
                if (target->selectable) {
                    selected = target;
                    input::event select;
                    select.gesture = platform::input::SELECT;
                    selected->events.raise(select);
                }
            }
        }

        virtual void spec(widget::spec spec, int w, int h, int margin) {
            config.spec = spec;
            config.w = w;
            config.h = h;
            config.margin = margin;
        }

        virtual void spec(int x, int y, widget::positioning relativity = widget::positioning::absolute) {
            config.relativity = relativity;
            config.x = x;
            config.y = y;
        }

        virtual void spec(int channel, int r, int g, int b, int a) {
            switch(channel) {
            case(0):
                config.background.r = r;
                config.background.g = g;
                config.background.b = b;
                config.background.a = a;
                break;
            case(1):
                config.edge.r = r;
                config.edge.g = g;
                config.edge.b = b;
                config.edge.a = a;
                break;
            }
        }

    protected:
        virtual void position(widget& instance) = 0;
        virtual void draw(widget& instance) = 0;

        std::map<int, widget *> instances;

        widget* selected = NULL;

        struct {
            widget::spec spec=widget::spec::none;
            widget::positioning relativity = widget::positioning::absolute;
            int x = 0;
            int y = 0;
            int w = 0;
            int h = 0;
            int margin = 0;
            struct {
                int r = 0;
                int g = 0;
                int b = 0;
                int a = 0;
            } background;
            struct {
                int r = 0;
                int g = 0;
                int b = 0;
                int a = 0;
            } edge;
        } config;
    };

}
