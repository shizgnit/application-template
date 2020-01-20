#pragma once

namespace type {

    class shader : virtual public type::info {
    public:
        std::string text;
        unsigned int context;
    };

}
