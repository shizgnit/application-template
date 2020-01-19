#pragma once

namespace format {

    class tga : public type::image {
    public:
        tga() {
            /* NULL */
        }

        tga(std::string filename) {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend std::istream& operator>>(std::istream& input, format::tga& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                return input;
            }

            // get the input size
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);

            // parse the header
            unsigned char header[18];

            input.read(header, 18);

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
              input.seekg(input.tellg() + instance.properties.id + instance.properties.colormap);
            }

            // get the raster data to parse
            size_t data = size - input.tellg();
            unsigned char* raster = new unsigned char[data];
            input.read(raster, data);

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
                        memcpy(pixel, buffer + source, Bpp);
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

            unsigned char* final = new unsigned char[pixel_count * 4];

            unsigned char* target;
            unsigned char* source;
            for (unsigned int y = 0; y < instance.properties.height; y++) {
                for (unsigned int x = 0; x < instance.properties.width; x++) {
                    //        target = final + (y * instance.properties.width * 4) + ((instance.properties.width - x) * 4);
                    //        target = final + ((instance.properties.height - y - 1) * instance.properties.width * 4) + (x * 4);
                    target = final + (y * instance.properties.width * 4) + (x * 4);
                    source = normalization + (y * instance.properties.width * Bpp) + (x * Bpp);
                    target[0] = source[2];
                    target[1] = source[1];
                    target[2] = source[0];
                    target[3] = Bpp == 4 ? source[4] : 255;
                }
            }

            delete[] normalization;

            instance.buffer.resize(allocation);
            instance.buffer.write(final, allocation);

            return input;
        }

    };

}