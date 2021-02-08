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
                    inuse.resize(supported, false);
                }

                GLenum allocate() {
                    for (int index = 0; index < inuse.size(); index++) {
                        if (inuse[index] == false) {
                            inuse[index] = true;
                            return(index & GL_COLOR_ATTACHMENT0);
                        }
                    }
                    return(0);
                }

                void release(GLenum allocation) {
                    inuse[allocation ^ GL_COLOR_ATTACHMENT0] = false;
                }

                std::vector<bool> inuse;
            };

            fbo(type::material& mat): texture(mat) { }

            bool init(bool depth = false);
            void enable();
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

            void compile(type::shader& shader);
            void compile(type::program& program);
            void compile(type::object& object);
            void compile(type::font& font);

            void draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection);
            void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection);

            void ontarget(type::object& object) {
                fx = new fbo(object.texture);
                fx->init(false);
                fx->enable();
            }

            void untarget() {
                fx->disable();
                geometry(display_width, display_height);
            }

            fbo* fx = NULL;
        };

    }

}
