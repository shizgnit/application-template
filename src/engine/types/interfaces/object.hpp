#pragma once

namespace type {

    class object : virtual public type::info {
    public:
        typedef spatial::vector::type_t type_t;

        void xy_projection(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
            type_t max_x = 0.0f;
            type_t max_y = 0.0f;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                if (vertices[i].coordinate.x > max_x) {
                    max_x = vertices[i].coordinate.x;
                }
                if (vertices[i].coordinate.y > max_y) {
                    max_y = vertices[i].coordinate.y;
                }
            }

            type_t texture_dx = 1 / (type_t)texture.map.properties.width;
            type_t texture_dy = 1 / (type_t)texture.map.properties.height;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                type_t dx = vertices[i].coordinate.x / max_x;
                type_t dy = (max_y - vertices[i].coordinate.y) / max_y;

                type_t tx = (width * dx + x) * texture_dx;
                type_t ty = (height * dy + y) * texture_dy;
                //float ty = 1.0f - ((height * dy + y) * texture_dy);

                vertices[i].texture(tx, ty, 0.0f, 0.0f);
            }

        }

        object* flat_normals() {
            int triangles = vertices.size() / 3;

            for (unsigned int i = 0; i < vertices.size() / 3; i++) {
                int index = i * 3;

                type_t u[3];
                type_t v[3];

                u[0] = vertices[index + 1].coordinate.x - vertices[index].coordinate.x;
                u[1] = vertices[index + 1].coordinate.y - vertices[index].coordinate.y;
                u[2] = vertices[index + 1].coordinate.z - vertices[index].coordinate.z;

                v[0] = vertices[index + 2].coordinate.x - vertices[index].coordinate.x;
                v[1] = vertices[index + 2].coordinate.y - vertices[index].coordinate.y;
                v[2] = vertices[index + 2].coordinate.z - vertices[index].coordinate.z;

                for (int j = index; j < index + 3; j++) {
                    vertices[j].normal.x = u[1] * v[2] - u[2] * v[1];
                    vertices[j].normal.y = u[2] * v[0] - u[0] * v[2];
                    vertices[j].normal.z = u[0] * v[1] - u[1] * v[0];
                    vertices[j].normal.w = 0.0f;
                }

            }

            return(NULL);
        }

        //std::vector<std::vector<spatial::vertex>> faces;

        spatial::geometry::primitive type;
        std::vector<spatial::vertex> vertices;

        type::material texture;

        std::string id;

        unsigned int context;

        // TODO: this needs to be replaced by bounding, either spheres or cubes
        int width() {
            return boundary_width;
        }
        int boundary_width;

        int height() {
            return boundary_height;
        }
        int boundary_height;

        object& operator = (const spatial::geometry& ref) {
            //TODO: this is making a really bad assumption based on the spatial::quad coordinate organization.
            boundary_width = ref.vertices[0].x;
            boundary_height = ref.vertices[0].y;
            std::copy(ref.vertices.begin(), ref.vertices.end(), std::back_inserter(this->vertices));
            type = ref.type;
            return *this;
        }

        friend type::object& operator>>(type::object& input, type::object& instance) {
            instance = input;
            return instance;
        }

        friend std::vector<type::object>& operator>>(type::object& input, std::vector<type::object>& instance) {
            instance = input.children;
            return instance;
        }

        std::vector<object> children;
    };

}