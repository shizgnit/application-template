#pragma once


#include <variant>

namespace format {

    class fbx : virtual public type::object {
    public:
        fbx() : type::info({ { "fbx" }, type::format::FORMAT_FBX }) {
            /* NULL */
        }

        fbx(std::string filename) : fbx() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        operator type::object& () {
            return(*this);
        }

        friend type::object& operator>>(std::istream& input, format::fbx& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            char buffer[1024];

            std::string reference = "Kaydara FBX Binary  ";
            input.read(buffer, 23);

            if (reference == buffer) {

            }

            int version = read<int>(input);

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }

    private:
        template<typename T> static T read(std::istream& input, size_t bytes = sizeof(T)) {
            char buffer[1024];
            input.read(buffer, bytes);
            if (typeid(int) == typeid(T)) {
                return atoi(buffer);
            }
            if (typeid(float) == typeid(T)) {
                return atof(buffer);
            }
        }
    };

    namespace parser {
        inline format::fbx fbx;
    }
}