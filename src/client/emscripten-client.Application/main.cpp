/*
================================================================================
  Copyright (c) 2023, Pandemos
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

#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "engine.hpp"
#include "application.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstddef>

// Geometry data to render and colors per-vertex
struct Vertex { 
   float x, y, z; 
   unsigned char r, g, b, a; 
};
std::vector<Vertex> g_vertices = {
   {  0.f,  .5f, 0.f,   255, 0, 0, 255 },
   { -.5f, -.5f, 0.f,   0, 255, 0, 255 },
   {  .5f, -.5f, 0.f,   0, 0, 255, 255 }
};

// Vertex shader that is called per-vertice
std::string g_vertexShader =
      "attribute vec4 a_position;          \n"
      "attribute vec4 a_color;             \n"
      "uniform float u_increment;          \n"
      "varying vec4 v_color;               \n"
      "void main() {                       \n"
      "    float s = sin(u_increment);     \n"
      "    float c = cos(u_increment);     \n"
      "    mat4 rot = mat4(                \n"
      "        c, -s, 0,  0,               \n"
      "        s,  c, 0,  0,               \n"
      "        0,  0, 1,  0,               \n"
      "        0,  0, 0,  1                \n"
      "    );                              \n"
      "    gl_Position = a_position * rot; \n"
      "    v_color = a_color;              \n"
      "}                                   \n";

// Fragment shader that is called per-pixel
std::string g_fragmentShader =
      "precision mediump float;            \n"
      "varying vec4 v_color;               \n"
      "void main() {                       \n"
      "    gl_FragColor = v_color;         \n"
      "}                                   \n";

// OpenGL object IDs
GLuint g_vertexShaderId = 0;
GLuint g_fragmentShaderId = 0;
GLuint g_shaderProgramId = 0;

GLuint g_shaderAttributePositionId = 0;
GLuint g_shaderAttributeColorId = 0;
GLuint g_shaderUniformIncrementId = 0;

GLuint g_vertexAttributeArrayId = 0;

// Small helper to compile shader from source string with retrieval of log on failure
static GLuint compileShaderWithLog(GLenum type, const std::string &source) {
   GLint status = GL_FALSE;
   GLuint id = glCreateShader(type);
   const char *str = source.c_str();
   glShaderSource(id, 1, &str, nullptr);
   glCompileShader(id);
   glGetShaderiv(id, GL_COMPILE_STATUS, &status);
   if(status != GL_TRUE) {
      GLint len = 0;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
      if(len > 0) {
         std::vector<char> buf(len);
         glGetShaderInfoLog(id, len, nullptr, buf.data());
         trace->error() << "Shader compile error: " << buf.data();
      }
   }
   return id;
}

// Shader attribute positions
GLuint g_shaderAttributePositionLoc = 0;
GLuint g_shaderAttributeColorLoc = 1;

// Compile a shader from source code
GLuint compileShader(GLenum type, const std::string &source, GLint *status=nullptr) {
   const GLuint id = glCreateShader(type);
   const char *str = source.c_str();
   glShaderSource(id, 1, &str, nullptr);
   glCompileShader(id);
   glGetShaderiv(id, GL_COMPILE_STATUS, status);
   return id;
};

inline application* instance = new app();

// Initialize the application
bool init() {
   int width = 640;
   int height = 640;

   // Set canvas size
   emscripten_set_canvas_element_size("#canvas", width, height);

   // Create a WebGL context
   EmscriptenWebGLContextAttributes attr;
   emscripten_webgl_init_context_attributes(&attr);
   attr.enableExtensionsByDefault = 1;
   // Use WebGL2 / OpenGL ES 3.0 so we can compile #version 300 es shaders
   attr.majorVersion = 2;
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
   emscripten_webgl_make_context_current(ctx);

   // Initialize the tracer
   trace->attach(std::make_shared<implementation::universal::console>(false));

   // Initialize the OpenGL render data
   trace->debug() << "Initialized OpenGL...";

   glClearColor(0.0f, 0.0f, 0.0f, 1.f);
   trace->debug() << "Set clear color to " << 0.0f << ", " << 0.0f << ", " << 0.0f << ", " << 1.0f;

	assets->init((void*)"/");
	assets->set("shader.path", std::string("shaders-emscripten"));

   instance->dimensions(width, height)->on_startup();
   trace->debug() << "Initialized Application...";

   // Compile vertex shader
   g_vertexShaderId = compileShader(GL_VERTEX_SHADER, g_vertexShader);
   printf("Vertext shader compiled, %d\n", g_vertexShaderId);

   // Compile fragment shader
   g_fragmentShaderId = compileShader(GL_FRAGMENT_SHADER, g_fragmentShader);
   printf("Fragment shader compiled, %d\n", g_fragmentShaderId);

   // Create shader program and attach shaders
   g_shaderProgramId = glCreateProgram();
   glAttachShader(g_shaderProgramId, g_vertexShaderId);
   glAttachShader(g_shaderProgramId, g_fragmentShaderId);
   glBindAttribLocation(g_shaderProgramId, g_shaderAttributePositionLoc, "a_position");
   glBindAttribLocation(g_shaderProgramId, g_shaderAttributeColorLoc, "a_color");
   glLinkProgram(g_shaderProgramId);
   glGetProgramiv(g_shaderProgramId, GL_LINK_STATUS, nullptr);
   g_shaderUniformIncrementId = glGetUniformLocation(g_shaderProgramId, "u_increment");
   printf("Shader compiled and linked, %d\n", g_shaderProgramId);

   glUseProgram(g_shaderProgramId);

   // Generate and bind vertex buffer
   glGenBuffers(1, &g_vertexAttributeArrayId);
   glBindBuffer(GL_ARRAY_BUFFER, g_vertexAttributeArrayId);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * g_vertices.size(), g_vertices.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(g_shaderAttributePositionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
   glEnableVertexAttribArray(g_shaderAttributePositionLoc);
   glVertexAttribPointer(g_shaderAttributeColorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(sizeof(float) * 3));
   glEnableVertexAttribArray(g_shaderAttributeColorLoc);
   printf("Geometry data uploaded, %d\n", g_vertexAttributeArrayId);

   return instance->started = true;
}

// Render a frame
void render() {
   if(instance->started) {
      instance->on_interval();
      instance->on_draw();
   }
   //if (instance->started) {
   //   auto& splash = assets->get<type::object>("splash");
   //   spatial::vector position = {
   //      (spatial::vector::type_t)(graphics->width() / 2) - (splash.width() / 2),
   //      (spatial::vector::type_t)graphics->height() - (graphics->height() / 2) - (splash.height() / 2)
   //   };
   //   trace->debug() << "Drawing splash at " << position.x << ", " << position.y;
   //   graphics->draw(splash, assets->get<type::program>("gui"), graphics->ortho, spatial::matrix(), spatial::matrix().translate(position));
   //   //graphics->compile(splash);
   //}
   // draw the cell instances created
}

// Handle mouse click events
bool mouse_click(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
   if (eventType == EMSCRIPTEN_EVENT_CLICK) {
      trace->debug() << "Mouse clicked at (" << mouseEvent->clientX << ", " << mouseEvent->clientY << ")";
   }
   return false;
}

int main() {
	if (init()) {
      trace->debug() << "Initialization complete";
      emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_click);
		//emscripten_set_main_loop(render, 0, 0);
      render();
	}
	return 0;
}



