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

// EM_JS for getting the left position
EM_JS(double, get_canvas_left, (), {
  const canvasElement = Module.canvas;
  if (canvasElement) {
    const rect = canvasElement.getBoundingClientRect();
    return rect.left;
  }
  return 0.0; // Return a default value if canvas not found
});

// EM_JS for getting the top position
EM_JS(double, get_canvas_top, (), {
  const canvasElement = Module.canvas;
  if (canvasElement) {
    const rect = canvasElement.getBoundingClientRect();
    return rect.top;
  }
  return 0.0; // Return a default value if canvas not found
});


#include "engine.hpp"
#include "application.hpp"


inline application* instance = new app();

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

// Track fullscreen state for toggle
static bool g_fullscreen = false;

// Resize any size-dependent framebuffers/textures (placeholder).
// If your renderer uses offscreen FBOs, recreate or reallocate them here.
static void resize_framebuffers(int w, int h) {
   trace->debug() << "Resizing framebuffers/textures to " << w << "x" << h;
   // TODO: Reallocate FBOs, textures, renderbuffers that depend on canvas size.
}

// Handle browser fullscreen change events and resize the canvas to the
// reported fullscreen dimensions. We update internal state from the event
// rather than relying on a keypress toggle.
bool fullscreen_change(int eventType, const EmscriptenFullscreenChangeEvent *evt, void *userData) {
   int canvasWidth, canvasHeight;

   // evt->isFullscreen is non-zero when in fullscreen mode
   bool isFs = evt ? (evt->isFullscreen != 0) : false;
   g_fullscreen = isFs;
   if (evt && evt->isFullscreen) {
      // When entering fullscreen, query the canvas backing size and update GL
      emscripten_get_canvas_element_size("#canvas", &canvasWidth, &canvasHeight);
      // Update app dimensions to the actual canvas size
      instance->dimensions(canvasWidth, canvasHeight);
      // Resize any size-dependent buffers
      resize_framebuffers(canvasWidth, canvasHeight);
      trace->debug() << "Entered fullscreen, canvas=" << canvasWidth << "x" << canvasHeight;
   } else {
      // Exiting fullscreen — restore to a reasonable default (same as init)
      int width = 640;
      int height = 640;
      emscripten_set_canvas_element_size("#canvas", width, height);
      instance->dimensions(width, height);
      resize_framebuffers(width, height);
      trace->debug() << "Exited fullscreen, restored canvas=" << width << "x" << height;
   }

   return false;
}

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

bool keyboard_event(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
   auto key = input->lookup_by_reference(keyEvent->code);
   if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
      trace->debug() << "Key down: " << keyEvent->key << " (code: " << key.code << ")";
      gui->raise({ platform::input::KEY, platform::input::DOWN, key.code, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
      input->raise({ platform::input::KEY, platform::input::DOWN, key.code, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });

      // Note: fullscreen state is now tracked via the browser fullscreenchange
      // event handler. Do not toggle fullscreen directly on keypress here.
   }
   if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
      trace->debug() << "Key up: " << keyEvent->key << " (code: " << key.code << ")";
      gui->raise({ platform::input::KEY, platform::input::UP, key.code, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
      input->raise({ platform::input::KEY, platform::input::UP, key.code, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
   }
   return false;
}

// Handle mouse click events
bool mouse_event(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
   static auto left = get_canvas_left();
   static auto top = get_canvas_top();

   struct { int x; int y; } p;
   p.x = mouseEvent->targetX - left;
   p.y = mouseEvent->targetY - top;

   auto button = mouseEvent->button + 1;
   if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
      trace->debug() << "Mouse down at (" << p.x << ", " << p.y << ")";
      if (gui->raise({ platform::input::POINTER, platform::input::DOWN, button, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
         input->raise({ platform::input::POINTER, platform::input::DOWN, button, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
      }
   }
   if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
      trace->debug() << "Mouse up at (" << p.x << ", " << p.y << ")";
      if (gui->raise({ platform::input::POINTER, platform::input::UP, button, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
         input->raise({ platform::input::POINTER, platform::input::UP, button, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
      }
   }
   if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
      //trace->debug() << "Mouse move at (" << mouseEvent->clientX << ", " << mouseEvent->clientY << ")";
      if (gui->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
         input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
      }
   }
   return false;
}

bool mouse_wheel(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData) {
   if (eventType == EMSCRIPTEN_EVENT_WHEEL) {
      trace->debug() << "Mouse wheel scrolled (" << wheelEvent->deltaY << ")";
      auto travel = wheelEvent->deltaY;
      if (gui->raise({ platform::input::POINTER, platform::input::WHEEL, 0, 0, (float)travel, { 0.0f, (float)travel, 0.0f } }, 0.0f, 0.0f) == false) {
         input->raise({ platform::input::POINTER, platform::input::WHEEL, 3, 0, (float)travel, { 0.0f, (float)travel, 0.0f } });
      }
   }
   return false;
}

int main() {
	if (init()) {
      trace->debug() << "Initialization complete";
      emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, keyboard_event);
      emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, keyboard_event);
      emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_event);
      emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_event);
      emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_event);
      emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_wheel);
      // Listen for browser fullscreen change events so we can react and resize
      // the canvas accordingly.
      emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, fullscreen_change);
		emscripten_set_main_loop(render, 0, 0);
      //render();
	}
	return 0;
}



