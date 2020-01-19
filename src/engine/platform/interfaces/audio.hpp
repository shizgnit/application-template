#pragma once

namespace platform {

    class audio {
    public:
        virtual void init(int sources) = 0;

        virtual void compile(type::audio& sound) = 0;

        virtual void play(type::audio& sound) = 0;

        virtual void shutdown(void) = 0;
    };

}