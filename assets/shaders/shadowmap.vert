#version 320 es

precision mediump float;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_LightingMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec4 a_Vertex;
in vec4 a_Texture;
in vec4 a_Normal;

out vec4 v_Vertex;
out vec2 v_Texture;
out vec4 v_Normal;
out vec4 v_Lighting;

void main()
{
  mat4 MVP = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix;

  v_Vertex = MVP * a_Vertex;
  v_Normal = normalize(MVP * a_Normal);
  
  gl_Position = v_Vertex;
  //gl_Position.z = (gl_Position.z / 10.0) - 1.0f;

  v_Lighting = (u_LightingMatrix * u_ModelMatrix) * a_Vertex;
}
