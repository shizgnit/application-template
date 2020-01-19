#pragma once

namespace platform {

    class assets {
    public:
        virtual void init(void *ref) { /*NULL*/ }

        virtual std::vector<std::string> list(std::string path) = 0;

        virtual std::istream& retrieve(std::string path) = 0;
    };

}
