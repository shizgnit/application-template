#pragma once

namespace platform {
    class graphics {
    public:
        enum render {
            UNDEFINED = 0x00,
            TEXTURE   = (1u << 0),
            WIREFRAME = (1u << 1),
            BOUNDS    = (1u << 2),
            NORMALS   = (1u << 3)
        };

        virtual void geometry(int width, int height) = 0;

        virtual void init(void) = 0;
        virtual void clear(void) = 0;
        virtual void flush(void) = 0;

        virtual void compile(type::shader& shader) = 0;
        virtual void compile(type::program& program) = 0;
        virtual void compile(type::object& object) = 0;
        virtual void compile(type::font& font) = 0;

        virtual void recompile(type::object& object) = 0;

        virtual void draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options=0x00) = 0;
        virtual void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options = 0x00) = 0;

        virtual void ontarget(type::object& object) {}
        virtual void untarget() {}

        typedef void (graphics::*callback)();
        utilities::scoped<graphics*, callback> target(type::object& object) {
            ontarget(object);
            return utilities::scoped<graphics*, callback>(this, &graphics::untarget);
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

    protected:
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