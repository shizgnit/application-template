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

        void create(char r, char g, char b, char a) {
            memset(&properties, 0, sizeof(properties));

            properties.width = 256;
            properties.height = 256;
            properties.bpp = 32;
            properties.depth = 8;
            properties.channels = 4;

            raster.resize(properties.width * properties.height * properties.channels);

            int index = 0;
            for (int width = 0; width < properties.width; width++) {
                for (int height = 0; height < properties.width; height++) {
                    raster[index++] = r;
                    raster[index++] = g;
                    raster[index++] = b;
                    raster[index++] = a;
                }
            }

        }
    };

}