#pragma once

namespace platform {

    class assets {
    public:
        struct source {
            std::string path;
            std::istream* stream;
        };

        virtual void init(void *ref) { /*NULL*/ }

        virtual std::vector<std::string> list(std::string path) = 0;

        virtual std::istream& retrieve(std::string path) = 0;

        virtual void release() = 0;

    protected:
        std::vector<assets::source> stack;
    };

}
