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
                none     = 0x00,
                left     = (1u << 0),
                right    = (1u << 1),
                initial  = (1u << 2),
                inherit  = (1u << 3),
                hcenter  = (1u << 4),
                vcenter  = (1u << 5),
                top      = (1u << 6),
                bottom   = (1u << 7),
                absolute = (1u << 8)
            };

            virtual widget& position(int x, int y, positioning relativity=positioning::none) {
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

            std::vector<widget*> children;

        protected:
            bool floating = false;
            positioning horizontal = positioning::none;
            positioning vertical = positioning::none;
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
            panel() : widget(widget::spec::panel) {
                this->id = platform::interface::next();
            }

            int add(interface::widget& child) {
                if (children.size()) {
                    child.position(children.back()->x + margin, margin);
                }
                else {
                    child.position(children.back()->x + margin, margin);
                }
                children.push_back(&child);
                return 0;
            }

            positioning alignment = positioning::right;
            float margin = 1.0;

        protected:
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

        virtual void raise(const input::event& ev, int x, int y) = 0;
        virtual void emit() = 0;

        virtual void position() = 0;
        virtual void draw() = 0;

        virtual widget& create(widget::spec t, int w, int h, int r, int g, int b, int a) = 0;
        virtual widget& create(widget* instance, int w, int h, int r, int g, int b, int a) = 0;

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

    protected:
        virtual void position(widget& instance) = 0;
        virtual void draw(widget& instance) = 0;

        std::map<int, widget *> instances;

        widget* selected = NULL;
    };

}