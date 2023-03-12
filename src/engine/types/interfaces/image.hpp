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
            type::info::operator=(ref);
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
