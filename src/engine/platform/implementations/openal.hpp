#pragma once

#define __PLATFORM_SUPPORTS_OPENAL 1

#include <AL/al.h>
#include <AL/alc.h>

namespace implementation {

    namespace openal {

        class audio : public platform::audio {
        public:
            void init(int sources);
            void compile(type::audio& sound);
            void play(type::audio& sound);
            void shutdown(void);

        protected:
            int sources;
            unsigned int* identifiers;
        };

    }

}
