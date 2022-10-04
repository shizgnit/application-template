#pragma once

namespace type {

    class sound : virtual public type::info {
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

        sound& operator=(const sound& ref) {
            memcpy(&properties, &ref.properties, sizeof(properties));
            buffer = ref.buffer;
            size = ref.size;
            return *this;
        }
        
        friend type::sound& operator>>(type::sound& input, type::sound& instance) {
            instance = input;
            return instance;
        }

        std::string type() {
            return "type::sound";
        }

        bool empty() {
            return buffer.empty();
        }
    };

}
