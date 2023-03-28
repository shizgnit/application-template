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

#define __PLATFORM_SUPPORTS_OPENGL 1

#if defined __PLATFORM_WINDOWS
#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\gl.h>
#include <GL\glu.h>
#endif

#if defined __PLATFORM_ANDROID
//#include <packages/glew.1.9.0.1/build/native/include/GL/glew.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#if defined __PLATFORM_IOS
#include <OpenGLES/gltypes.h>
#include <OpenGLES/ES3/gl.h>
#endif

#if defined __PLATFORM_MACOS
#include "OpenGL/gl3.h"
#endif

#if !defined GL_MAX_COLOR_ATTACHMENTS
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#endif

std::string glGetErrorString(GLenum err);

#define GL_REPORT_ERROR(_call, _error) \
    ::graphics->event(utilities::string() << #_call << ", " << _error << ", " << __LINE__ << ", " << __FILE__)

#if defined _DEBUG_
#define GL_TEST(_call) \
    { \
        GLenum _error; \
        while((_error = glGetError()) != GL_NO_ERROR) { } \
        _call; \
        std::stringstream str; \
        int errors = 0; \
        while((_error = glGetError()) != GL_NO_ERROR) { \
            str << glGetErrorString(_error); \
            errors += 1; \
        } \
        if(errors) GL_REPORT_ERROR(_call, str.str()); \
    }
#else
#define GL_TEST(_call) _call;
#endif

class type::info::opaque_t {
public:
    std::map<type::program*, unsigned int> vao;
    unsigned int context;
};

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

            bool deinit();
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

            utilities::seconds_t timestamp = std::chrono::system_clock::now().time_since_epoch();
            GLuint pbo = 0;
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
            std::vector<type::program*> renderer;

            std::map<unsigned int, fbo> fbos;

            int offset_vector = 0;
            int offset_matrix = 0;
        };

    }

}
