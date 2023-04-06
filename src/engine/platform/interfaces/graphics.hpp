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

namespace platform {

    class graphics : public properties {
    public:
        enum render {
            UNDEFINED = 0x00,
            TEXTURE   = (1u << 0),
            WIREFRAME = (1u << 1),
            BOUNDS    = (1u << 2),
            NORMALS   = (1u << 3)
        };

        virtual void dimensions(int width, int height, float fov=90.0, float scale=1.0) = 0;

        virtual void init(void) = 0;
        virtual void clear(void) = 0;
        virtual void flush(void) = 0;

        virtual bool compile(type::shader& shader) = 0;
        virtual bool compile(type::program& program) = 0;
        virtual bool compile(type::material& material) = 0;
        virtual bool compile(type::object& object) = 0;
        virtual bool compile(type::font& font) = 0;

        virtual bool compile(platform::assets* assets) = 0;

        virtual void draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00) = 0;
        virtual void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00) = 0;

        virtual void ontarget(type::object& object) {}
        virtual void untarget() {}

        virtual void oninvert() {}
        virtual void uninvert() {}

        virtual void release(type::object& object) {}

        typedef void (graphics::* callback)();
        utilities::scoped<graphics*, callback> target(type::object& object) {
            ontarget(object);
            return utilities::scoped<graphics*, callback>(this, &graphics::untarget);
        }

        utilities::scoped<graphics*, callback> invert() {
            oninvert();
            return utilities::scoped<graphics*, callback>(this, &graphics::uninvert);
        }

        virtual void clip(float top, float bottom, float left, float right) {
            clip_top.push_back(top);
            clip_bottom.push_back(bottom);
            clip_left.push_back(left);
            clip_right.push_back(right);
        }

        virtual void noclip() {
            if (clip_top.size() > 1) {
                clip_top.pop_back();
            }
            if (clip_bottom.size() > 1) {
                clip_bottom.pop_back();
            }
            if (clip_left.size() > 1) {
                clip_left.pop_back();
            }
            if (clip_right.size() > 1) {
                clip_right.pop_back();
            }
        }

        int height() {
            return display_height;
        }

        int width() {
            return display_width;
        }
        
        struct {
            spatial::position position;
            spatial::vector color = { 1.0, 1.0, 1.0, 1.0 };
            float bias = 0.0f;
            float strength = 0.0f;
        } ambient;

        struct {
            spatial::position directional;
        } light;

        type::object ray;

        // Render buffers
        type::object shadow;
        type::object color;
        type::object depth;
        type::object blur;
        type::object picking;

        spatial::matrix ortho;
        spatial::matrix perspective;

        std::vector<unsigned char> pixels;

        spatial::matrix::type_t parameter(int index) {
            int col = index / 4;
            int row = index % 4;
            return parameters[col][row];
        }

        void parameter(int index, spatial::matrix::type_t value) {
            int col = index / 4;
            int row = index % 4;
            parameters.set(row, col, value);
        }

        std::string stat() {
            if (frames.size() == 0) {
                return "";
            }
            std::stringstream ss;
            ss << frames.back().frames;
            return ss.str();
        }
        
        spatial::position* reference = NULL;

    protected:
        spatial::matrix parameters;

        std::vector<float> clip_top = { 10000.0f };
        std::vector<float> clip_bottom = { 10000.0f };
        std::vector<float> clip_left = { 10000.0f };
        std::vector<float> clip_right = { 10000.0f };

        int display_width;
        int display_height;

        time_t timestamp;
        struct stats {
            void clear() { memset(this, 0, sizeof(frame)); }
            int vertices = 0;
            int lines = 0;
            int triangles = 0;
            int frames = 0;
        } frame;

        std::vector<stats> frames;
        std::list<stats> activity;
    };
}
