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
            //typedef void(*callback)(const platform::input::event&);
            typedef std::function<void(const platform::input::event&)> callback;

            enum spec {
                button,
                textbox,
                tabbed,
                progress
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

            virtual widget& handler(platform::input::type t, platform::input::action a, callback c, int identifier = 0) {
                callbacks[t][a][identifier] = c;
                return *this;
            }

            virtual widget& raise(const input::event& ev) {
                if (callbacks.find(ev.input) != callbacks.end()) {
                    if (callbacks[ev.input].find(ev.gesture) != callbacks[ev.input].end()) {
                        if (callbacks[ev.input][ev.gesture].find(ev.identifier) != callbacks[ev.input][ev.gesture].end()) {
                            callbacks[ev.input][ev.gesture][ev.identifier](ev);
                        }
                        /// Also fire off the catch-all handler
                        if (callbacks[ev.input][ev.gesture].find(0) != callbacks[ev.input][ev.gesture].end()) {
                            callbacks[ev.input][ev.gesture][0](ev);
                        }
                    }
                }
                return *this;
            }

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

        protected:
            bool floating = false;
            positioning horizontal = positioning::none;
            positioning vertical = positioning::none;

            std::vector<std::reference_wrapper<widget>> children;

            std::map<platform::input::type, std::map<platform::input::action, std::map<int, callback>>> callbacks;
        };

        class button : public widget {
        public:
            button() : widget(widget::spec::button) { 
                this->id = platform::interface::next();
            }

            std::string label;

            ::type::object icon;
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

        class tabbed : public widget {
        public:
            tabbed() : widget(widget::spec::tabbed) {
                this->id = platform::interface::next();
            }

            int add(interface::widget& button, interface::widget& content) {
                children.push_back(button);
                crossreference[" "] = children.size();
                children.push_back(content);
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

    protected:
        virtual void position(widget& instance) = 0;
        virtual void draw(widget& instance) = 0;

        std::map<int, widget *> instances;

        widget* selected = NULL;
    };

}