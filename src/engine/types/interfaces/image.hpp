#pragma once

namespace type {

    class image : virtual public type::info {
    public:
        struct {
            unsigned int type;
            unsigned int width;
            unsigned int height;
            unsigned char bpp;
            unsigned char depth;
            unsigned char channels;
            unsigned char id;
            unsigned int x_offset;
            unsigned int y_offset;
            unsigned char descriptor;
            unsigned int colormap;
        } properties;

        std::vector<char> colormap;
        std::vector<char> raster;

        friend type::image& operator>>(type::image& input, type::image& instance) {
            instance = input;
            return instance;
        }
    };

}