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

namespace format {

    class mtl : virtual public type::material {
    public:
        mtl() : type::info({ { "mtl" }, type::format::FORMAT_MTL }) {
            /* NULL */
        }

        mtl(std::string filename) : mtl() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        operator type::material& () {
            return(*this);
        }

        friend type::material& operator>>(std::istream& input, format::mtl& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
                return instance;
            }

            /*
            newmtl meat
            Ns 100.000
            d 1.00000
            illum 2
            Kd 0.567100 0.567100 0.567100
            Ka 0.766900 0.766900 0.766900
            Ks 1.00000 1.00000 1.00000
            Ke 1.00000e-3 0.00000e+0 0.00000e+0
            map_Kd meat.tga
            */

            bool parse = false;
            type::material* material = NULL;

            std::string line;
            while (std::getline(input, line)) {
                auto arguments = utilities::tokenize(utilities::rtrim(line), " ");
                auto command = arguments[0];

                if (command == "newmtl") {
                    std::string id = assets->resolve(arguments[1]);
                    parse = assets->has<type::material>(id) == false;
                    material = &assets->get<type::material>(id);
                    instance.children.push_back(assets->get<type::material>(id));
                }

                if (parse == false) {
                    continue;
                }

                if (command == "Ka") {
                    material->ambient[0] = atof(arguments[1].c_str());
                    material->ambient[1] = atof(arguments[2].c_str());
                    material->ambient[2] = atof(arguments[3].c_str());
                    material->ambient[3] = 1.0f;
                    material->specular[0] = atof(arguments[1].c_str());
                    material->specular[1] = atof(arguments[2].c_str());
                    material->specular[2] = atof(arguments[3].c_str());
                    material->specular[3] = 1.0f;
                }
                if (command == "Kd") {
                    material->diffuse[0] = atof(arguments[1].c_str());
                    material->diffuse[1] = atof(arguments[2].c_str());
                    material->diffuse[2] = atof(arguments[3].c_str());
                    material->diffuse[3] = 1.0f;
                }
                if (command == "Ke") {
                    material->emission[0] = atof(arguments[1].c_str());
                    material->emission[1] = atof(arguments[2].c_str());
                    material->emission[2] = atof(arguments[3].c_str());
                    material->emission[3] = 1.0f;
                }
                if (command == "Ns") {
                    material->shininess = atof(arguments[1].c_str());
                }
                if (command == "d") {
                    material->opacity = atof(arguments[1].c_str());
                }
                if (command == "illum") {
                    material->illumination = atof(arguments[1].c_str());
                }
                if (command == "map_Kd") {
                    auto texture = assets->load("texture", arguments[1] == "." ? "untitled.png" : arguments[1]);
                    material->color = &assets->get<type::image>(texture);
                }
                if (command == "map_Bump") {
                    auto texture = assets->load("texture", arguments[1] == "." ? "untitled.png" : arguments[1]);
                    material->normal = &assets->get<type::image>(texture);
                }
            }

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::mtl mtl;
    }
}