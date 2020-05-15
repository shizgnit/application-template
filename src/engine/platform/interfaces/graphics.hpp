#pragma once

namespace platform {
    class graphics {
    public:
        virtual void geometry(int width, int height) = 0;

        virtual void init(void) = 0;
        virtual void clear(void) = 0;
        virtual void flush(void) = 0;

        virtual void compile(type::shader& shader) = 0;
        virtual void compile(type::program& program) = 0;
        virtual void compile(type::object& object) = 0;
        virtual void compile(type::font& font) = 0;

        virtual void draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) = 0;
        virtual void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) = 0;

        virtual void clip(float top, float bottom, float left, float right) {
            clip_top = top;
            clip_bottom = bottom;
            clip_left = left;
            clip_right = right;
        }

        virtual void noclip() {
            clip_top = 10000.0f;
            clip_bottom = 10000.0f;
            clip_left = 10000.0f;
            clip_right = 10000.0f;
        }

        int height() {
            return display_height;
        }

        int width() {
            return display_width;
        }

    protected:
        float clip_top = 10000.0f;
        float clip_bottom = 10000.0f;
        float clip_left = 10000.0f;
        float clip_right = 10000.0f;

        int display_width;
        int display_height;
    };
}