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

        image& operator=(const image& ref) {
            memcpy(&properties, &ref.properties, sizeof(properties));
            colormap = ref.colormap;
            raster = ref.raster;
            return *this;
        }
        
        friend type::image& operator>>(type::image& input, type::image* instance) {
            *instance = input;
            return *instance;
        }

        friend type::image& operator>>(type::image& input, type::image& instance) {
            instance = input;
            return instance;
        }

        void create(int width, int height, char r, char g, char b, char a) {
            memset(&properties, 0, sizeof(properties));

            r = r > 255 ? 255 : r;
            g = g > 255 ? 255 : g;
            b = b > 255 ? 255 : b;
            a = a > 255 ? 255 : a;

            properties.width = width;
            properties.height = height;
            properties.bpp = 32;
            properties.depth = 8;
            properties.channels = 4;

            raster.resize(properties.width * properties.height * properties.channels);

            int index = 0;
            for (int w = 0; w < properties.width; w++) {
                for (int h = 0; h < properties.height; h++) {
                    raster[index++] = r;
                    raster[index++] = g;
                    raster[index++] = b;
                    raster[index++] = a;
                }
            }

            compiled(false);
        }

        std::string type() {
            return "type::image";
        }

        bool empty() {
            return raster.empty();
        }
    };

}
