#pragma once


#include <variant>

#include "zlib.h"

// https://code.blender.org/2013/08/fbx-binary-file-format-specification/

namespace format {

    class fbx : virtual public type::object {
    public:
        class node {
        public:
            typedef std::variant<char, short, int, float, double, long long, std::string, char *> property;

            node() {}

            void parse(std::istream& input, unsigned int end=0) {
                if (end == 0) {
                    end = utilities::read<int>(input);
                    unsigned int count = utilities::read<int>(input);

                    utilities::read<int>(input); // property size, just ignore

                    name = utilities::read<std::string>(input, utilities::read<char>(input));

                    values(input, count);
                }
                auto location = input.tellg();
                while (input.tellg() < end) {
                    children.resize(children.size() + 1);
                    children.back().parse(input);
                    location = input.tellg();
                }
            }

            void values(std::istream& input, int count, char spec = '?') {
                for (int i = 0; i < count; i++) {
                    char type = spec == '?' ? utilities::read<char>(input) : spec;

                    switch (type) {

                        // scalar values
                    case('Y'):
                        properties.push_back(utilities::read<short>(input));
                        break;
                    case('C'):
                        properties.push_back(utilities::read<char>(input));
                        break;
                    case('I'):
                        properties.push_back(utilities::read<int>(input));
                        break;
                    case('F'):
                        properties.push_back(utilities::read<float>(input));
                        break;
                    case('D'):
                        properties.push_back(utilities::read<double>(input));
                        break;
                    case('L'):
                        properties.push_back(utilities::read<long long>(input));
                        break;
                    case('S'):
                        properties.push_back(utilities::read<std::string>(input, utilities::read<unsigned int>(input)));
                        break;
                    case('R'):
                        properties.push_back(utilities::read<std::string>(input, utilities::read<unsigned int>(input))); // This is just temporary
                        // properties.push_back(read<char*>(input, read<unsigned int>(input)));
                        break;

                        // assume array
                    default:
                        auto location = input.tellg();

                        unsigned int elements = utilities::read<unsigned int>(input);
                        unsigned int compressed = utilities::read<unsigned int>(input);
                        unsigned int compressed_size = utilities::read<unsigned int>(input);

                        unsigned int uncompressed_size;
                        switch (type) {
                        case('y'):
                            uncompressed_size = elements * 2;
                            break;
                        case('c'):
                        case('b'):
                            uncompressed_size = elements;
                            break;
                        case('i'):
                        case('f'):
                            uncompressed_size = elements * 4;
                            break;
                        case('d'):
                        case('l'):
                            uncompressed_size = elements * 8;
                            break;
                        }

                        std::vector<char> compressed_buffer(compressed_size, 0);
                        std::vector<char> uncompressed_buffer(uncompressed_size, 0);
                        if (compressed) {
                            input.read(compressed_buffer.data(), compressed_size);

                            uncompress((unsigned char*)uncompressed_buffer.data(), (unsigned long *)&uncompressed_size, (unsigned char*)compressed_buffer.data(), compressed_size);

                            std::stringstream stream;
                            std::copy(uncompressed_buffer.begin(), uncompressed_buffer.end(), std::ostream_iterator<char>(stream, ""));

                            values(stream, elements, toupper(type));
                        }
                        else {
                            values(input, elements, toupper(type));
                        }

                        break;
                    }
                }
            }

            std::string name;

            std::vector<property> properties;

            std::vector<node> children;
        };

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
            /// Ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            // Get the input size before consumption starts
            input.seekg(0, input.end);
            unsigned int size = input.tellg();
            input.seekg(0, input.beg);

            char buffer[1024];

            std::string reference = "Kaydara FBX Binary  ";
            input.read(buffer, 23);

            if (reference != buffer) {
                return instance;
            }

            int version = utilities::read<int>(input);

            node document;
            document.parse(input, size);

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::fbx fbx;
    }
}