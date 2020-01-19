#pragma once

namespace type {

    class material {
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
    };

}