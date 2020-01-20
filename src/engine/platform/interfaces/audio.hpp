#pragma once

namespace platform {

    class audio {
    protected:
        const static int default_channels = 4;

    public:
        virtual void init(int sources = audio::default_channels) = 0;

        virtual void compile(type::audio& sound) = 0;

        virtual void play(type::audio& sound) = 0;

        virtual void shutdown(void) = 0;
    };

}