#pragma once

#define __PLATFORM_SUPPORTS_METAL 1


namespace implementation {

    namespace metal {

        #define BUFFER_OFFSET(i) ((void*)(i))

        static inline float deg_to_radf(float deg) {
            return deg * (float)M_PI / 180.0f;
        }

        class graphics : public platform::graphics {
        public:
            void projection(int fov);
            void dimensions(int width, int height, float scale=1.0);

            void init(void);
            void clear(void);
            void flush(void);

            bool compile(type::shader& shader);
            bool compile(type::program& program);
            bool compile(type::material& material);
            bool compile(type::object& object);
            bool compile(type::font& font);
            bool compile(type::entity& entity);

            bool compile(platform::assets* assets);

            void draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options=0x00);
            void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00);

            void ontarget(type::object& object);
            void untarget();

            void oninvert();
            void uninvert();

            void release(type::object* object) {
            }

            std::vector<type::object*> target;

            int offset_vector = 0;
            int offset_matrix = 0;
        };

    }

}
