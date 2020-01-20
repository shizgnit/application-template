#pragma once

namespace type {

    class object : virtual public type::info {
    public:
        class vertex {
        public:
            float coordinate[4];
            float texture[4];
            float normal[4];

            void set_coordinate(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
                coordinate[0] = x;
                coordinate[1] = y;
                coordinate[2] = z;
                coordinate[3] = w;
            }
            void set_texture(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
                texture[0] = x;
                texture[1] = y;
                texture[2] = z;
                texture[3] = w;
            }
            void set_normal(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
                normal[0] = x;
                normal[1] = y;
                normal[2] = z;
                normal[3] = w;
            }

            operator bool() const {
                return(true);
            }
        };

        void xy_projection(type::image &reference, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
            texture.map = reference;

            float max_x = 0.0f;
            float max_y = 0.0f;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                if (vertices[i].coordinate[0] > max_x) {
                    max_x = vertices[i].coordinate[0];
                }
                if (vertices[i].coordinate[1] > max_y) {
                    max_y = vertices[i].coordinate[1];
                }
            }

            float texture_dx = 1 / (float)texture.map.properties.width;
            float texture_dy = 1 / (float)texture.map.properties.height;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                float dx = vertices[i].coordinate[0] / max_x;
                float dy = (max_y - vertices[i].coordinate[1]) / max_y;

                float tx = (width * dx + x) * texture_dx;
                float ty = (height * dy + y) * texture_dy;
                //float ty = 1.0f - ((height * dy + y) * texture_dy);

                vertices[i].texture[0] = tx;
                vertices[i].texture[1] = ty;
                vertices[i].texture[2] = 0.0f;
                vertices[i].texture[3] = 0.0f;
            }

        }

        object* flat_normals() {
            int triangles = vertices.size() / 3;

            for (unsigned int i = 0; i < vertices.size() / 3; i++) {
                int index = i * 3;

                float u[3];
                float v[3];

                u[0] = vertices[index + 1].coordinate[0] - vertices[index].coordinate[0];
                u[1] = vertices[index + 1].coordinate[1] - vertices[index].coordinate[1];
                u[2] = vertices[index + 1].coordinate[2] - vertices[index].coordinate[2];

                v[0] = vertices[index + 2].coordinate[0] - vertices[index].coordinate[0];
                v[1] = vertices[index + 2].coordinate[1] - vertices[index].coordinate[1];
                v[2] = vertices[index + 2].coordinate[2] - vertices[index].coordinate[2];

                for (int j = index; j < index + 3; j++) {
                    vertices[j].normal[0] = u[1] * v[2] - u[2] * v[1];
                    vertices[j].normal[1] = u[2] * v[0] - u[0] * v[2];
                    vertices[j].normal[2] = u[0] * v[1] - u[1] * v[0];
                    vertices[j].normal[3] = 0.0f;
                }

            }

            return(NULL);
        }

        std::vector<vertex> vertices;

        std::vector<std::vector<vertex>> faces;

        type::material texture;

        std::string id;

        unsigned int context;

        static struct primitive {

            static type::object quad(float width, float height) {
                int factor = 1;

                type::object obj;
                obj.vertices.reserve(6 * factor * factor);

                float x = 0.0f;
                float y = 0.0f;

                float dx = width / factor;
                float dy = height / factor;

                int index = 0;

                for (int i = 0; i < factor; i++) {
                    x = dx * i;
                    for (int j = 0; j < factor; j++) {
                        y = dy * j;

                        obj.vertices[index++].set_coordinate(x + dx, y + dy);
                        obj.vertices[index++].set_coordinate(x, y + dy);
                        obj.vertices[index++].set_coordinate(x, y);

                        obj.vertices[index++].set_coordinate(x + dx, y + dy);
                        obj.vertices[index++].set_coordinate(x, y);
                        obj.vertices[index++].set_coordinate(x + dx, y);
                    }
                }

                return(obj);
            }

        };

        friend type::object& operator>>(type::object& input, type::object& instance) {
            instance = input;
            return instance;
        }
    };

}