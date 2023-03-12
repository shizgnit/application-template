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

    class material : virtual public type::info {
    public:
        /* TODO: the pointers may be reused... fix this
        ~material() {
            if (color) {
                delete color;
                color = NULL;
            }
        }
        */

        float ambient[4];
        float diffuse[4];
        float specular[4];
        float emission[4];

        float shininess;
        float opacity;
        float illumination;

        type::image* normal = NULL;
        type::image* color = NULL;
        type::image* blur = NULL;

        bool depth = false;

        operator type::image* () {
            if (color == NULL) {
                color = new type::image;
            }
            return color;
        }

        void create(int width, int height, char r, char g, char b, char a) {
            color = new type::image;
            color->create(width, height, r, g, b, a);
            compiled(false);
        }

        friend type::material& operator>>(type::material& input, type::material& instance) {
            instance = input;
            return instance;
        }

        friend std::vector<type::material>& operator>>(type::material& input, std::vector<type::material>& instance) {
            instance = input.children;
            return instance;
        }

        std::string type() {
            return "type::material";
        }

        bool empty() {
            return children.empty();
        }

    protected:
        std::vector<material> children;
    };

}