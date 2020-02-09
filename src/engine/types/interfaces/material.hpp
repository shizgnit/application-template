#pragma once

namespace type {

    class material : virtual public type::info {
    public:
        std::string id;

        float ambient[4];
        float diffuse[4];
        float specular[4];
        float emission[4];

        float shininess;
        float opacity;
        float illumination;

        type::image map;
        unsigned int context;

        friend type::material& operator>>(type::material& input, type::material& instance) {
            instance = input;
            return instance;
        }

        friend std::vector<type::material>& operator>>(type::material& input, std::vector<type::material>& instance) {
            instance = input.children;
            return instance;
        }

    protected:
        std::vector<material> children;
    };

}