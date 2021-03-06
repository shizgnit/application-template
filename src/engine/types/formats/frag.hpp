#pragma once

namespace format {

    class frag : virtual public type::shader {
    public:
        frag() : type::info({ { "frag", "fsh" }, type::format::FORMAT_FRAG }) {
            /* NULL */
        }

        frag(std::string filename) : frag() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend type::shader& operator>>(std::istream& input, format::frag& instance) {
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

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::frag frag;
    }
}
