/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

//#include "deps/fbx-master/src/fbxdocument.h"

#include <variant>
#include <miniz-2.2.0/miniz.h>

//#include "zlib.h"

// https://code.blender.org/2013/08/fbx-binary-file-format-specification/

namespace format {

    class fbx : virtual public type::object {
    public:
        class node {
        public:
            typedef std::variant<char, short, int, float, double, long long, std::string, char *> property;

            node() {}

            // 385

            // 380 - 160

            bool parse(std::istream& input, unsigned int end=0) {
                if (end == 0) {
                    end = utilities::read<int>(input);
                    if (end == 0x9ABBCFA) {
                        auto location = input.tellg();
                        return false; // hit the footer
                    }
                    unsigned int count = utilities::read<int>(input);
                    utilities::read<int>(input); // property size, just ignore
                    name = utilities::read<std::string>(input, utilities::read<char>(input));
                    values(input, count);
                }
                auto location = input.tellg();
                if (location > 78880) {
                    int y = 0;
                }

                bool done = false;
                while (input.tellg() < end && !done) {
                    children.resize(children.size() + 1);
                    done = children.back().parse(input) == false;
                }

                return true;
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
                            if (compressed_size == 0 || compressed_size > 74000) {
                                int x = 0;
                            }

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
#ifdef _WIN32
            //::fbx::FBXDocument doc;
            //doc.read(filename);
            //doc.print();
#endif
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                this->read(file);
            }
        }

        operator type::object& () {
            return(*this);
        }

        type::object& read(std::istream& input) {
            if (input.good() == false) {
                return *this;
            }

            // Get the input size before consumption starts
            input.seekg(0, input.end);
            unsigned int size = input.tellg();
            input.seekg(0, input.beg);

            char buffer[1024];

            std::string reference = "Kaydara FBX Binary  ";
            input.read(buffer, 23);

            if (reference != buffer) {
                return *this;
            }

            int version = utilities::read<int>(input);

            node document;
            document.parse(input, size);

            // Done parsing the incoming asset
            assets->release();

            // TODO: Need to translate the document to the object members!

            return *this;
        }

        friend type::object& operator>>(std::istream& input, format::fbx& instance) {
            instance.read(input);
            return instance;
        }
    };

    namespace parser {
        inline format::fbx fbx;
    }
}