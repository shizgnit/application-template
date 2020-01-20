#pragma once

namespace format {

    class vert : virtual public type::shader {
    public:
        vert(): type::info({ { "vert", "vsh" }, type::format::FORMAT_VERT }) {
            /* NULL */
        }

        vert(std::string filename) : vert() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend type::shader& operator>>(std::istream& input, format::vert& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            /// Read in the input stream
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);
            instance.text.resize(size);
            input.read(instance.text.data(), size);

            return instance;
        }
    };

    namespace parser {
        inline format::vert vert;
    }
}
