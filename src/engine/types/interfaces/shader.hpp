#pragma once

namespace type {

    class shader : public type::info {
    public:
        std::vector<char> buffer;
        unsigned int context;
    };

}
