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
                auto arguments = utilities::tokenize(line, " ");
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
                    material->map = &assets->get<type::image>(texture);
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