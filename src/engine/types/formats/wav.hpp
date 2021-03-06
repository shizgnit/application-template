#pragma once

namespace format {

    class wav : public type::audio {
    public:
        wav() : type::info({ { "wav" }, type::format::FORMAT_WAV }) {
            /* NULL */
        }

        wav(std::string filename) : wav() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend type::audio& operator>>(std::istream& input, format::wav &instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            /// Read in the input stream
            input.seekg(0, std::ios::end);
            instance.size = input.tellg();
            input.seekg(0, std::ios::beg);
            instance.buffer.resize(instance.size);
            input.read(instance.buffer.data(), instance.size);

            /// Define the header
            struct {
                char chunk[4];
                unsigned int chunk_size;
                char format[4];
                char subchunk[4];
                unsigned int subchunk_size;
                unsigned short audio_format;
                unsigned short channels;
                unsigned int sample_rate;
                unsigned int byte_rate;
                unsigned short block_align;
                unsigned short bits_per_sample;
                char data[4];
                unsigned int data_size;
            } header;

            /// Pull out the data
            memcpy(&header, instance.buffer.data(), sizeof(header));

            /// Translate to the type properties
            instance.properties.sample_rate = header.sample_rate;
            instance.properties.bits_per_sample = header.bits_per_sample;
            instance.properties.block_align = header.block_align;
            instance.properties.channels = header.channels;
            instance.properties.size = header.data_size;

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::wav wav;
    }
}
