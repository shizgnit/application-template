/*
================================================================================
  Copyright (c) 2023, Pandemos
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

namespace format {

    class tga : virtual public type::image {
    public:
        tga() : type::info({ { "tga" }, type::format::FORMAT_TGA }) {
            /* NULL */
        }

        tga(std::string filename) : tga() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend std::istream& operator>>(std::istream& input, format::tga& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
                return input;
            }

            // get the input size
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);

            // parse the header
            unsigned char header[18];

            input.read((char*)header, 18);

            instance.properties.type = header[2];

            //    0 - No image data included.
            //    1 - Uncompressed, color - mapped images.
            //    2 - Uncompressed, RGB images.
            //    3 - Uncompressed, blackand white images.
            //    9 - Runlength encoded color - mapped images.
            //    10 - Runlength encoded RGB images.
            //    11 - Compressed, blackand white images.
            //    32 - Compressed color - mapped data, using Huffman, Delta, and
            //    runlength encoding.
            //    33 - Compressed color - mapped data, using Huffman, Delta, and
            //    runlength encoding.  4 - pass quadtree - type process.

            instance.properties.width = header[12] | (header[13] << 8);
            instance.properties.height = header[14] | (header[15] << 8);
            instance.properties.bpp = static_cast<unsigned char>(header[16]);
            instance.properties.id = static_cast<unsigned char>(header[0]);
            instance.properties.x_offset = header[8] | (header[9] << 8);
            instance.properties.y_offset = header[10] | (header[11] << 8);
            instance.properties.descriptor = static_cast<unsigned char>(header[17]);

            instance.properties.colormap = (header[5] << 8) | header[6];

            // ignore colormaps for now
            if (instance.properties.colormap) {
              input.seekg((off_t)input.tellg() + (off_t)instance.properties.id + (off_t)instance.properties.colormap);
            }

            // get the raster data to parse
            off_t data = size - input.tellg();
            unsigned char* raster = new unsigned char[data];
            input.read((char *)raster, data);

            // parse the raster data
            unsigned char order = (instance.properties.descriptor & 0x30) >> 4;

            unsigned char byte;
            unsigned int Bpp = instance.properties.bpp / 8;
            unsigned int pixel_count = instance.properties.width * instance.properties.height;
            unsigned int allocation = pixel_count * (Bpp);

            unsigned char* temporary = new unsigned char[allocation];

            if (instance.properties.type == 9 || instance.properties.type == 10) {
                unsigned int target = 0;
                unsigned int source = 0;
                unsigned char* pixel = new unsigned char[Bpp];
                while (target < allocation) {
                    if (target >= 766000) {
                        target = target;
                    }
                    byte = raster[source++];
                    unsigned int count = (byte & 0x7F) + 1;
                    if (byte & 0x80) {
                        memcpy(pixel, raster + source, Bpp);
                        source += Bpp;
                        for (unsigned int i = 0; i < count; i++) {
                            memcpy(temporary + target, pixel, Bpp);
                            target += Bpp;
                        }
                    }
                    else {
                        unsigned int size = count * Bpp;
                        memcpy(temporary + target, raster + source, size);
                        target += size;
                        source += size;
                    }
                }
                delete[] pixel;
            }
            else {
                memcpy(temporary, raster, allocation);
            }

            // normalize data orientation
            unsigned char* normalization = NULL;
            if (instance.properties.descriptor & 0x20) {
                normalization = new unsigned char[allocation];
                unsigned int back = allocation;
                for (unsigned int i = 0; i < allocation; i++) {
                    normalization[--back] = temporary[i];
                }
                delete[](temporary);
            }
            else {
                normalization = temporary;
            }

            unsigned char* normalized = new unsigned char[pixel_count * 4];

            unsigned char* target;
            unsigned char* source;
            for (unsigned int y = 0; y < instance.properties.height; y++) {
                for (unsigned int x = 0; x < instance.properties.width; x++) {
                    //        target = final + (y * instance.properties.width * 4) + ((instance.properties.width - x) * 4);
                    //        target = final + ((instance.properties.height - y - 1) * instance.properties.width * 4) + (x * 4);
                    target = normalized + (y * instance.properties.width * 4) + (x * 4);
                    source = normalization + (y * instance.properties.width * Bpp) + (x * Bpp);
                    target[0] = source[2];
                    target[1] = source[1];
                    target[2] = source[0];
                    target[3] = Bpp == 4 ? source[4] : 255;
                }
            }

            delete[] normalization;

            instance.raster.resize(allocation);
            memcpy(instance.raster.data(), normalized, allocation);

            delete[] normalized;

            // Done parsing the incoming asset
            assets->release();

            return input;
        }

    };

    namespace parser {
        inline format::tga tga;
    }
}