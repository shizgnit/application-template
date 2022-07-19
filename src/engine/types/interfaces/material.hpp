#pragma once

namespace type {

    class material : virtual public type::info {
    public:
        /* TODO: the pointers may be reused... fix this
        ~material() {
            if (color) {
                delete color;
                color = NULL;
            }
        }
        */

        float ambient[4];
        float diffuse[4];
        float specular[4];
        float emission[4];

        float shininess;
        float opacity;
        float illumination;

        type::image* normal = NULL;
        type::image* color = NULL;
        type::image* blur = NULL;

        bool depth = false;

        operator type::image* () {
            if (color == NULL) {
                color = new type::image;
            }
            return color;
        }

        void create(int width, int height, char r, char g, char b, char a) {
            color = new type::image;
            color->create(width, height, r, g, b, a);
            compiled(false);
        }

        friend type::material& operator>>(type::material& input, type::material& instance) {
            instance = input;
            return instance;
        }

        friend std::vector<type::material>& operator>>(type::material& input, std::vector<type::material>& instance) {
            instance = input.children;
            return instance;
        }

        std::string type() {
            return "type::material";
        }

        bool empty() {
            return children.empty();
        }

    protected:
        std::vector<material> children;
    };

}