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

            std::string line;
            while (std::getline(input, line)) {
                auto arguments = utilities::tokenize(line, " ");
                auto command = arguments[0];

                if (command == "newmtl") {
                    instance.children.push_back(type::material());
                    instance.children.back().id = arguments[1];
                }
                if (command == "Ka") {
                    instance.children.back().ambient[0] = atof(arguments[1].c_str());
                    instance.children.back().ambient[1] = atof(arguments[2].c_str());
                    instance.children.back().ambient[2] = atof(arguments[3].c_str());
                    instance.children.back().ambient[3] = 1.0f;
                    instance.children.back().specular[0] = atof(arguments[1].c_str());
                    instance.children.back().specular[1] = atof(arguments[2].c_str());
                    instance.children.back().specular[2] = atof(arguments[3].c_str());
                    instance.children.back().specular[3] = 1.0f;
                }
                if (command == "Kd") {
                    instance.children.back().diffuse[0] = atof(arguments[1].c_str());
                    instance.children.back().diffuse[1] = atof(arguments[2].c_str());
                    instance.children.back().diffuse[2] = atof(arguments[3].c_str());
                    instance.children.back().diffuse[3] = 1.0f;
                }
                if (command == "Ke") {
                    instance.children.back().emission[0] = atof(arguments[1].c_str());
                    instance.children.back().emission[1] = atof(arguments[2].c_str());
                    instance.children.back().emission[2] = atof(arguments[3].c_str());
                    instance.children.back().emission[3] = 1.0f;
                }
                if (command == "Ns") {
                    instance.children.back().shininess = atof(arguments[1].c_str());
                }
                if (command == "d") {
                    instance.children.back().opacity = atof(arguments[1].c_str());
                }
                if (command == "illum") {
                    instance.children.back().illumination = atof(arguments[1].c_str());
                }
                if (command == "map_Kd") {
                    type::image texture;
                    assets->retrieve(arguments[1]) >> format::parser::png >> texture;
                    instance.children.back().map = texture;
                }
            }

            return instance;
        }
    };

    namespace parser {
        inline format::mtl mtl;
    }
}