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
            void on_press(const event& ev);
            void on_release(const event& ev);
            void on_move(const event& ev);

            void on_key_up(const event& ev);
            void on_key_down(const event& ev);

            void on_button_up(const event& ev);
            void on_button_down(const event& ev);

            bool drag;

            std::mutex tracking; 
        };

        class interface : public platform::interface {
        public:
            void raise(const input::event& ev, int x, int y);
            void emit();

            void draw();

            widget& create(widget::spec t, int w, int h, const std::string& texture);
            widget& create(widget::spec t, int w, int h, int r, int g, int b, int a);

            void print(int x, int y, const std::string& text);

        protected:
            void draw(widget& instance);
            void reposition(widget& instance);
        };

    }
}