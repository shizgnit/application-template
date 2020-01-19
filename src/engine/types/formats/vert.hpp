#pragma once

namespace format {

    class frag : public type::shader {
    public:
        frag() {
            /* NULL */
        }

        frag(std::string filename) {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend std::istream& operator>>(std::istream& input, format::frag& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return input;
            }

            /// Read in the input stream
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);
            instance.buffer.resize(size);
            input.read(instance.buffer.data(), size);

            return input;
        }
    };
}
