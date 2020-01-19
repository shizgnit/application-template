#pragma once

#define __PLATFORM_SUPPORTS_OPENSL 1

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace implementation {

    namespace opensl {

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
