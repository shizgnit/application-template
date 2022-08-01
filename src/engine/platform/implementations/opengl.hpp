#pragma once

#define __PLATFORM_SUPPORTS_OPENGL 1

#if defined __PLATFORM_WINDOWS
#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\gl.h>
#include <GL\glu.h>
#endif

#if defined __PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#if defined __PLATFORM_IOS
#include <OpenGLES/gltypes.h>
#include <OpenGLES/ES3/gl.h>
#endif

#if !defined GL_MAX_COLOR_ATTACHMENTS
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#endif


namespace implementation {

    namespace opengl {

        #define BUFFER_OFFSET(i) ((void*)(i))

        static inline float deg_to_radf(float deg) {
            return deg * (float)M_PI / 180.0f;
        }

        class fbo {
        public:
            class attachment {
            public:
                int supported = 0;
                
                attachment() {
                    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &supported);
                    for (int i = 0; i < supported; i++) {
                        inuse[GL_COLOR_ATTACHMENT0 + i] = false;
                    }
                }

                GLenum allocate() {
                    for (auto entry: inuse) {
                        if (entry.second == false) {
                            entry.second = true;
                            return(entry.first);
                        }
                    }
                    return(0);
                }

                void release(GLenum allocation) {
                    inuse[allocation] = false;
                }

                std::map<GLenum, bool> inuse;
            };

            static attachment& attachments() {
                static attachment instance;
                return instance;
            }

            fbo() { }

            bool init(type::object& object, platform::graphics *ref, bool depth = false, unsigned char *collector=NULL);

            void enable(bool clear = false);
            void disable();

        private:
            struct {
                unsigned int frame = 0;
                unsigned int render = 0;
                unsigned int depth = 0;
            } context;

            GLenum allocation = 0;

            platform::graphics* parent = NULL;

            type::object* target = NULL;

            unsigned char* collect = NULL;
        };

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
                fbos.erase(object->instance);
            }

            std::vector<type::object*> target;

            std::map<unsigned int, fbo> fbos;

            int offset_vector = 0;
            int offset_matrix = 0;
        };

    }

}
