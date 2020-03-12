#pragma once


#include <variant>

// https://code.blender.org/2013/08/fbx-binary-file-format-specification/

namespace format {

    class fbx : virtual public type::object {

        class node {
            typedef std::variant<short, char, int, float, double, long long, std::string, char *> property;

            property scalar(std::istream& input) {
                return property();
            }

            node(std::istream& input) {

                unsigned int size = read<int>(input);
                unsigned int count = read<int>(input);
                
                read<int>(input); // property size, just ignore

                name = read<std::string>(input, read<char>(input));

                for (int i=0; i < count; i++) {
                    char type = read<char>(input);

                    switch (type) {

                    // scalar values
                    case('Y'):
                        properties.push_back(read<short>(input));
                        break;
                    case('C'):
                        properties.push_back(read<char>(input));
                        break;
                    case('I'):
                        properties.push_back(read<int>(input));
                        break;
                    case('F'):
                        properties.push_back(read<float>(input));
                        break;
                    case('D'):
                        properties.push_back(read<double>(input));
                        break;
                    case('L'):
                        properties.push_back(read<long long>(input));
                        break;
                    case('S'):
                        properties.push_back(read<std::string>(input, read<unsigned int>(input)));
                        break;
                    case('R'):
                        properties.push_back(read<char *>(input, read<unsigned int>(input)));
                        break;

                    // assume array
                    default:
                        unsigned int elements = read<unsigned int>(input);
                        unsigned int encoding = read<unsigned int>(input);
                        unsigned int compressed = read<unsigned int>(input);

                        // TODO : conditional zstream decompress and move scalar read to a method

                        break;
                        
                    }
                }

            }

            std::vector<property> properties;

            std::string name;
        };

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

            if (reference != buffer) {
                return instance;
            }

            int version = read<int>(input);

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }

    private:
        template<class T> static T read(std::istream& input, size_t bytes = sizeof(T)) {
            char buffer[8];
            input.read(buffer, bytes);
            return *reinterpret_cast<T *>(buffer);
        }

        template<std::string &t> static std::string read(std::istream& input, size_t bytes) {
            std::vector<char> buffer(bytes);
            input.read(&buffer[0], bytes);
            buffer[bytes] = '\0';
            return std::string(&buffer[0]);
        }

        template<char * t> static std::string read(std::istream& input, size_t bytes) {
            char* memory = new char[bytes];  // TODO: do not allocate here
            input.read(memory, bytes);
            return memory;
        }

    };

    namespace parser {
        inline format::fbx fbx;
    }
}