#pragma once

namespace format {

    class obj : virtual public type::object {
    public:
        obj() : type::info({ { "obj" }, type::format::FORMAT_OBJ }) {
            /* NULL */
        }

        obj(std::string filename) : obj() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        operator type::object& () {
            return(*this);
        }

        friend type::object& operator>>(std::istream& input, format::obj& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            /*
            mtllib apple.mtl
            o sphere5
            #20 vertices, 24 faces
            v 0.77081294 1.37713812 0.18578580
            vt 0.0000000e+0 0.83301356
            vn 0.56772715 -0.18355389 0.80249228
            g sphere5_leaf
            usemtl leaf
            f 1/3/1 7/13/7 12/16/12 6/10/6
            ...
            o cylinder3
            #176 vertices, 204 faces
            v -2.3757008e-2 1.51884413 -3.6220710e-18
            ...
            */

            std::vector<std::vector<float>> coordinates;
            std::vector<std::vector<float>> textures;
            std::vector<std::vector<float>> normals;

            std::vector<type::material> mats;

            std::string line;
            while (std::getline(input, line)) {
                auto arguments = utilities::tokenize(line, " ");
                auto command = arguments[0];

                if (command == "mtllib") { //material
                    assets->retrieve(arguments[1]) >> format::parser::mtl >> mats;
                }
                if (command == "o") { //entity
                    instance.children.push_back(type::object());
                    instance.children.back().id = arguments[1];
                }
                if (command == "v") { //vertex
                    coordinates.push_back({
                        (float)atof(arguments[1].c_str()),
                        (float)atof(arguments[2].c_str()),
                        (float)atof(arguments[3].c_str())
                    });
                }
                if (command == "vt") { //texture coordinates
                    textures.push_back({
                        (float)atof(arguments[1].c_str()),
                        (float)atof(arguments[2].c_str())
                    });
                }
                if (command == "vn") { //normals
                    normals.push_back({
                        (float)atof(arguments[1].c_str()),
                        (float)atof(arguments[2].c_str()),
                        (float)atof(arguments[3].c_str())
                    });
                }
                if (command == "g") { //group
                }
                if (command == "usemtl") { //material
                    for (auto mat : mats) {
                        if (mat.id == arguments[1]) {
                            instance.children.back().texture = mat;
                        }
                    }
                }
                if (command == "f") { //faces
                    if (arguments.size() >= 4) {
                        std::vector<object::vertex> vertices;

                        for (unsigned int i = 1; i < arguments.size(); i++) {
                            type::object::vertex vertex;

                            auto points = utilities::tokenize(arguments[i], "/");

                            int vi = (atoi(points[0].c_str()) - 1);
                            int ti = (atoi(points[1].c_str()) - 1);
                            int ni = (atoi(points[2].c_str()) - 1);

                            vertex.coordinate[0] = coordinates[vi][0];
                            vertex.coordinate[1] = coordinates[vi][1];
                            vertex.coordinate[2] = coordinates[vi][2];
                            vertex.coordinate[3] = 1.0f;

                            vertex.texture[0] = textures[ti][0];
                            vertex.texture[1] = textures[ti][1];
                            vertex.texture[2] = 0.0f;
                            vertex.texture[3] = 0.0f;

                            vertex.normal[0] = normals[ni][0];
                            vertex.normal[1] = normals[ni][1];
                            vertex.normal[2] = normals[ni][2];
                            vertex.normal[3] = 0.0f;

                            vertices.push_back(vertex);
                        }

                        if (vertices.size() == 3) {
                            instance.children.back().vertices.push_back(vertices[0]);
                            instance.children.back().vertices.push_back(vertices[1]);
                            instance.children.back().vertices.push_back(vertices[2]);
                        }
                        if (vertices.size() == 4) {
                            instance.children.back().vertices.push_back(vertices[0]);
                            instance.children.back().vertices.push_back(vertices[1]);
                            instance.children.back().vertices.push_back(vertices[2]);

                            instance.children.back().vertices.push_back(vertices[0]);
                            instance.children.back().vertices.push_back(vertices[2]);
                            instance.children.back().vertices.push_back(vertices[3]);
                        }
                    }

                }
            }

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::obj obj;
    }
}