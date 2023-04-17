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

    class wav : public type::sound {
    public:
        wav() : type::info({ { "wav" }, type::format::FORMAT_WAV }) {
            /* NULL */
        }

        wav(std::string filename) : wav() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        friend type::sound& operator>>(std::istream& input, format::wav &instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
                return instance;
            }

            /// Read in the input stream
            input.seekg(0, std::ios::end);
            instance.size = input.tellg();
            input.seekg(0, std::ios::beg);
            instance.buffer.resize(instance.size);
            input.read(instance.buffer.data(), instance.size);

            /// Define the header
            struct {
                char chunk[4];
                unsigned int chunk_size;
                char format[4];
                char subchunk[4];
                unsigned int subchunk_size;
                unsigned short audio_format;
                unsigned short channels;
                unsigned int sample_rate;
                unsigned int byte_rate;
                unsigned short block_align;
                unsigned short bits_per_sample;
                char data[4];
                unsigned int data_size;
            } header;

            /// Pull out the data
            memcpy(&header, instance.buffer.data(), sizeof(header));

            /// Translate to the type properties
            instance.properties.sample_rate = header.sample_rate;
            instance.properties.bits_per_sample = header.bits_per_sample;
            instance.properties.block_align = header.block_align;
            instance.properties.channels = header.channels;
            instance.properties.size = header.data_size;

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::wav wav;
    }
}
