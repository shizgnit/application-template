#pragma once

namespace type {

    class material : virtual public type::info {
    public:
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