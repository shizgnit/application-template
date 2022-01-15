#pragma once

#define __PLATFORM_SUPPORTS_OPENGL 1

#if defined __PLATFORM_WINDOWS
#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\gl.h>
#include <GL\glu.h>
#endif

#if defined __PLATFORM_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
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
                attachment() {
                    int supported;
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

            fbo(type::material& mat): texture(mat) { }

            bool init(bool depth = false);
            void enable(bool clear = false);
            void disable();

            private:
                struct {
                    GLuint frame;
                    GLuint render;
                    GLuint depth;
                } context;

                GLenum allocation;
                type::material &texture;
        };

        class graphics : public platform::graphics {
        public:
            void geometry(int width, int height);

            void init(void);
            void clear(void);
            void flush(void);

            bool compile(type::shader& shader);
            bool compile(type::program& program);
            bool compile(type::object& object);
            bool compile(type::font& font);

            bool recompile(type::object& object);

            void draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options=0x00);
            void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options = 0x00);

            void ontarget(type::object& object) {
                if (fx == NULL) {
                    fx = new fbo(object.texture);
                    fx->init();
                }
                fx->enable();
            }

            void untarget() {
                fx->disable();
                geometry(display_width, display_height);
            }

            fbo* fx = NULL;

            type::object ray;
        };

    }

}
