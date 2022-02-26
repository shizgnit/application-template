#version 320 es

precision mediump float;

layout(location = 0) in mat4 a_ModelMatrix;

layout(location = 4) in vec4 a_Vertex;

layout(location = 7) uniform mat4 u_ModelMatrix;
layout(location = 8) uniform mat4 u_ViewMatrix;
layout(location = 9) uniform mat4 u_ProjectionMatrix;

void main()
{
  mat4 MVP = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix;
  if(gl_InstanceID >= 1) {
    MVP = u_ProjectionMatrix * u_ViewMatrix * a_ModelMatrix;
  }

  gl_Position = MVP * a_Vertex;
}
