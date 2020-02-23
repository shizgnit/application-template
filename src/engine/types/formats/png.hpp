#pragma once

#include "deps/lodepng/lodepng.h"

namespace format {

    class png : public type::image {
    public:
        png() : type::info({ { "png" }, type::format::FORMAT_PNG }) {
            /* NULL */
        }

        png(std::string filename) : png() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        operator type::image& () {
            return(*this);
        }

        friend type::image& operator>>(std::istream& input, format::png& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return instance;
            }

            /// get the input size
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);

            std::vector<unsigned char> png;
            std::vector<unsigned char> raster;
            unsigned width, height;

            /// load from input
            png.resize(size);
            input.read((char *)png.data(), size);

            /// decode
            lodepng::State state;
            auto error = lodepng::decode(raster, width, height, state, png);
            if (error) {}

            /// interpret the info
            auto color = state.info_png.color;
            instance.properties.width = width;
            instance.properties.height = height;
            instance.properties.bpp = lodepng_get_bpp(&color);
            instance.properties.depth = color.bitdepth;
            instance.properties.channels = lodepng_get_channels(&color);

            size_t allocation = width * height * instance.properties.bpp;

            instance.raster.resize(allocation);
            memcpy(instance.raster.data(), raster.data(), raster.size());

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::png png;
    }
}