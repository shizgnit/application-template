#pragma once

namespace type {

    class audio {
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
    };

}