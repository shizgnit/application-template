#pragma once

#define __PLATFORM_ANDROID 1

namespace implementation {

    namespace android {

        class assets : public platform::assets {
        public:
            void init(void* ref);

            std::vector<std::string> list(const std::string& path);

            std::istream& retrieve(const std::string& path);

            void release();

            std::string load(const std::string& type, const std::string& resource, const std::string& id = "");

        private:
            AAssetManager* assetManager;
        };

    }

}


