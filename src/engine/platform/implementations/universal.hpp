#pragma once

#define __PLATFORM_UNIVERSAL 1

namespace implementation {

    namespace universal {

        class input : public platform::input {
        public:
            void raise(const event& ev);
            void emit();

            std::string printable(int vkey) {
                return keys[16].pressed || keys[160].pressed ? keys[vkey].meta : keys[vkey].character;
            }

        protected:
            int active_pointer();
            int active_pointer(const event& ev);

            void on_press(const event& ev);
            void on_release(const event& ev);
            void on_move(const event& ev);

            void on_key_up(const event& ev);
            void on_key_down(const event& ev);

            void on_button_up(const event& ev);
            void on_button_down(const event& ev);

            bool drag;

            std::mutex tracking;

            std::vector<spatial::vector> points;
        };

        class interface : public platform::interface {
        public:
            bool raise(const input::event& ev, int x, int y);
            void emit();

            void position();
            void draw();

            widget* create(std::vector<widget*>& c);
            widget* create(widget::spec t, int w, int h, int r, int g, int b, int a);
            widget* create(widget* instance, int w, int h, int r, int g, int b, int a);

            void print(int x, int y, const std::string& text);

        protected:
            void draw(widget& instance);
            void position(widget& instance);
        };

        class assets : public platform::assets::common {
        public:
            std::string load(platform::assets*, const std::string& type, const std::string& resource, const std::string& id);
        };

    }
}