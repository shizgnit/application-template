#pragma once

#define __PLATFORM_SUPPORTS_OPENAL 1

#if defined __PLATFORM_APPLE
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace implementation {

    namespace openal {

        class audio : public platform::audio {
        public:
            void init(int sources);
            void compile(type::audio& sound);
            void shutdown(void);

            int start(type::audio& sound);
            void stop(int id);

        protected:
            int sources;
            unsigned int* identifiers;
        };

    }

}
