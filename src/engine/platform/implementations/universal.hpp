#pragma once

#define __PLATFORM_UNIVERSAL 1

namespace implementation {

    namespace universal {

        class input : public platform::input {
        public:
            void raise(const event& ev);
            void emit();

        protected:
            void on_press(const event& ev);
            void on_release(const event& ev);
            void on_move(const event& ev);

            void on_key_up(const event& ev);
            void on_key_down(const event& ev);
        };

    }
}