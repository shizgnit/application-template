#pragma once

#define __PLATFORM_ANDROID 1

namespace implementation {

    namespace android {

        class assets : public platform::assets {
        public:
            void init(void* ref);

            std::vector<std::string> list(std::string path);

            std::istream& retrieve(std::string path);

        private:
            AAssetManager* assetManager;
        };

    }

}


