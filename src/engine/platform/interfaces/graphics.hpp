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

        virtual void projection(int fov) = 0;
        virtual void dimensions(int width, int height, float scale=1.0) = 0;

        virtual void init(void) = 0;
        virtual void clear(void) = 0;
        virtual void flush(void) = 0;

        virtual bool compile(type::shader& shader) = 0;
        virtual bool compile(type::program& program) = 0;
        virtual bool compile(type::material& material) = 0;
        virtual bool compile(type::object& object) = 0;
        virtual bool compile(type::font& font) = 0;
        virtual bool compile(type::entity& entity) = 0;

        virtual bool compile(platform::assets* assets) = 0;

        virtual void draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00) = 0;
        virtual void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00) = 0;

        virtual void ontarget(type::object* object) {}
        virtual void untarget() {}

        virtual void oninvert() {}
        virtual void uninvert() {}

        virtual int messages() {
            return errors.size();
        }

        virtual std::string message() {
            if (errors.size() == 0) {
                return "";
            }
            std::string status = errors.front();
            errors.pop_front();
            return status;
        }

        typedef void (graphics::* callback)();
        utilities::scoped<graphics*, callback> target(type::object& object) {
            ontarget(&object);
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
            spatial::vector color;
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

    protected:
        spatial::matrix parameters;

        std::list<std::string> errors;

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