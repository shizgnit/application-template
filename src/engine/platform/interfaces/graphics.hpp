#pragma once

namespace platform {
    class graphics {
    public:
        virtual void init(void) = 0;
        virtual void clear(void) = 0;

        virtual void compile(type::shader& shader) = 0;
        virtual void compile(type::program& program) = 0;
        virtual void compile(type::object& object) = 0;
        virtual void compile(type::font& font) = 0;

        virtual void draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) = 0;
        virtual void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) = 0;
    };
}