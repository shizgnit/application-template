uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_LightingMatrix;
uniform mat4 u_LightProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

attribute vec4 a_Vertex;
attribute vec4 a_Texture;
attribute vec4 a_Normal;

varying vec4 v_Vertex;
varying vec2 v_Texture;
varying vec4 v_Normal;
varying vec4 v_Lighting;

void main()
{
  mat4 MVP = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix;

  v_Texture = a_Texture.xy;
  v_Vertex = MVP * a_Vertex;
  v_Normal = normalize(MVP * a_Normal);
  
  gl_Position = v_Vertex;
  gl_Position.z = -1.0 + (gl_Position.z / 10.0);

  v_Lighting = (u_LightingMatrix * u_ModelMatrix) * a_Vertex;
}
