/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

#include "lodepng/lodepng.h"

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
                assets->release();
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