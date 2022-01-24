#pragma once

namespace type {

    class audio : virtual public type::info {
    public:
        struct {
            unsigned short channels;
            unsigned int sample_rate;
            unsigned int byte_rate;
            unsigned short block_align;
            unsigned short bits_per_sample;
            unsigned int size;
        } properties;

        std::vector<char> buffer;
        unsigned int size = 0;
        unsigned int context;

        friend type::audio& operator>>(type::audio& input, type::audio& instance) {
            instance = input;
            return instance;
        }

        bool empty() {
            return buffer.empty();
        }
    };

}