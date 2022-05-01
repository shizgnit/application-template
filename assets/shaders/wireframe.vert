#version 320 es

precision mediump float;
precision mediump int;

layout(location = 0) in mat4 a_ModelMatrix;

layout(location = 4) in vec4 a_Vertex;
layout(location = 5) in vec4 a_Texture;
layout(location = 6) in vec4 a_Normal;

layout(location = 7) uniform mat4 u_ModelMatrix;
layout(location = 8) uniform mat4 u_ViewMatrix;
layout(location = 9) uniform mat4 u_ProjectionMatrix;
layout(location = 10) uniform mat4 u_LightingMatrix;

layout(location = 11) uniform vec4 u_Clipping;

layout(location = 12) uniform vec4 u_AmbientLightPosition;
layout(location = 13) uniform vec4 u_AmbientLightColor;

layout(location = 14) uniform uint u_Flags;

out vec4 v_Vertex;
out vec2 v_Texture;
out vec4 v_Normal;
out vec4 v_Lighting;
out vec4 v_Clipping;

out float v_FrontFacing;

void main()
{
  mat4 MVP = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix;

  if(gl_InstanceID >= 1) {
    MVP = u_ProjectionMatrix * u_ViewMatrix * a_ModelMatrix;
  }

  v_Texture = a_Texture.xy;
  v_Vertex = MVP * a_Vertex;
  v_Normal = normalize(MVP * a_Normal);
  
  v_FrontFacing = dot(v_Normal, normalize(v_Vertex));

  gl_Position = v_Vertex;
}