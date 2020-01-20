#pragma once

namespace type {

    class shader : virtual public type::info {
    public:
        std::string text;
        unsigned int context;

        friend type::shader& operator>>(type::shader& input, type::shader& instance) {
            instance = input;
            return instance;
        }
    };

}
