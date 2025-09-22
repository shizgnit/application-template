#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>

#include <string>
#include <vector>

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
   // Set canvas size
   emscripten_set_canvas_element_size("#canvas", 640, 480);

   // Create a WebGL context
   EmscriptenWebGLContextAttributes attr;
   emscripten_webgl_init_context_attributes(&attr);
   attr.enableExtensionsByDefault = 1;
   attr.majorVersion = 1;
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
   emscripten_webgl_make_context_current(ctx);

   // Initialize the OpenGL render data
   printf("Initialize OpenGL\n");

   glClearColor(0.0f, 0.0f, 0.0f, 1.f);

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

   return true;
}

// Render a frame
void render() {
   static int increment = 0;
   if(increment >= 1000.f) {
      increment = 0;
   }
   glUniform1f(g_shaderUniformIncrementId, ++increment / 1000.f);
   glDrawArrays(GL_TRIANGLES, 0, 3);
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



