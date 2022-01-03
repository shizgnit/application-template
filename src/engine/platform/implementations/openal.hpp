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
            void shutdown(void);

            int start(type::audio& sound);
            void stop(int id);

        protected:
            int sources;
            unsigned int* identifiers;
        };

    }

}
