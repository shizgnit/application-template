#pragma once

namespace implementation {

    namespace macos {

        class assets : public platform::assets {
        public:
            void init(void* ref=NULL);

            std::vector<std::string> list(const std::string& path);

            std::istream& retrieve(const std::string& path);

            void release();

            std::string load(const std::string& type, const std::string& resource, const std::string& id = "");

            std::string base;
        };

    }

}


