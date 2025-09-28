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
   attr.majorVersion = 1;
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
   emscripten_webgl_make_context_current(ctx);

   // Initialize the OpenGL render data
   printf("Initialized OpenGL\n");

   glClearColor(0.0f, 0.0f, 0.0f, 1.f);

   instance->dimensions(width, height)->on_startup();

   return instance->started = true;
}

// Render a frame
void render() {
   if (instance->started) {
      instance->on_interval();
      instance->on_draw();
   }
}

// Handle mouse click events
bool mouse_click(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
   if (eventType == EMSCRIPTEN_EVENT_CLICK) {
      printf("Mouse clicked at (%d, %d)\n", mouseEvent->clientX, mouseEvent->clientY);
   }
   return false;
}

int main() {
	if (init()) {
      emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, mouse_click);
		emscripten_set_main_loop(render, 0, 0);
	}
	return 0;
}



