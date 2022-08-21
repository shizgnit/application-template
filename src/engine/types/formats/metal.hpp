#pragma once

namespace format {

    class metal : virtual public type::shader {
    public:
        metal(): type::info({ { "metal", "metal" }, type::format::FORMAT_METAL }) {
            /* NULL */
        }

        metal(std::string filename) : metal() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend type::shader& operator>>(std::istream& input, format::metal& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
                return instance;
            }

            /// Read in the input stream
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);
            instance.text.resize(size);
            input.read(instance.text.data(), size);

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::metal metal;
    }
}
