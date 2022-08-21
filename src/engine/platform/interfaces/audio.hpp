#pragma once

namespace platform {

    class audio {
    protected:
        const static int default_channels = 4;

    public:
        virtual void init(int sources = audio::default_channels) = 0;

        virtual void compile(type::sound& sound) = 0;

        virtual void shutdown(void) = 0;

        virtual int start(type::sound& sound) = 0;
        virtual void stop(int id) = 0;

    };

}