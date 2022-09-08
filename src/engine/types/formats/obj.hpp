#pragma once

namespace format {

    class obj : virtual public type::object {
        std::string decorator;
        bool material = true;

        spatial::vector offset;

    public:
        typedef spatial::vector::type_t type_t;

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

        format::obj& d(const std::string& value) {
            decorator = value;
            return *this;
        }

        format::obj& g() {
            material = false;
            return *this;
        }

        format::obj& o(spatial::vector& amount) {
            offset = amount;
            return *this;
        }

        friend type::object& operator>>(std::istream& input, format::obj& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
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

            instance.children.clear();

            std::vector<spatial::vector> coordinates;
            std::vector<spatial::vector> textures;
            std::vector<spatial::vector> normals;

            std::string mtllib;

            bool parse = false;

            std::string line;
            while (std::getline(input, line)) {
                auto arguments = utilities::tokenize(utilities::rtrim(line), " ");
                auto command = arguments[0];

                if (command == "mtllib") { //material
                    mtllib = assets->load("material", arguments[1]);
                }
                if (command == "o") { //entity
                    std::string id = assets->resolve(instance.decorator + arguments[1]);
                    parse = assets->has<type::object>(id) == false;
                    instance.children.push_back(assets->get<type::object>(id));
                    coordinates.clear();
                    textures.clear();
                    normals.clear();
                }

                if (parse == false) {
                    continue;
                }

                if (command == "v") { //vertex
                    coordinates.push_back(spatial::vector({
                        (type_t)atof(arguments[1].c_str()),
                        (type_t)atof(arguments[2].c_str()),
                        (type_t)atof(arguments[3].c_str())
                    }) + instance.offset);
                }
                if (command == "vt") { //texture coordinates
                    textures.push_back({
                        (type_t)atof(arguments[1].c_str()),
                        (type_t)atof(arguments[2].c_str())
                    });
                }
                if (command == "vn") { //normals
                    normals.push_back({
                        (type_t)atof(arguments[1].c_str()),
                        (type_t)atof(arguments[2].c_str()),
                        (type_t)atof(arguments[3].c_str())
                    });
                }
                if (command == "g") { //group
                }
                if (command == "usemtl") { //material
                    std::string id = assets->resolve(arguments[1]);
                    if (assets->has<type::material>(id) == false) {
                        assets->event(utilities::string() << id << " failed to load reference material");
                    }
                    instance.children.back().texture = assets->get<type::material>(id);
                }
                if (command == "f") { //faces
                    if (arguments.size() >= 4) {
                        std::vector<spatial::vertex> vertices;

                        for (unsigned int i = 1; i < arguments.size(); i++) {
                            spatial::vertex vertex;

                            auto points = utilities::tokenize(arguments[i], "/");

                            int vi = (atoi(points[0].c_str()) - 1);
                            int ti = (atoi(points[1].c_str()) - 1);
                            int ni = (atoi(points[2].c_str()) - 1);

                            vertex.coordinate.x = coordinates[vi].x;
                            vertex.coordinate.y = coordinates[vi].y;
                            vertex.coordinate.z = coordinates[vi].z;
                            vertex.coordinate.w = 1.0f;

                            vertex.texture.x = textures[ti].x;
                            vertex.texture.y = 1.0f - textures[ti].y;
                            vertex.texture.z = 0.0f;
                            vertex.texture.w = 0.0f;

                            vertex.normal.x = normals[ni].x;
                            vertex.normal.y = normals[ni].y;
                            vertex.normal.z = normals[ni].z;
                            vertex.normal.w = 0.0f;

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
