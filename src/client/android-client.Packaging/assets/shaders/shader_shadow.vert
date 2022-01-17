uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

attribute vec4 a_Vertex;
attribute vec4 a_Texture;
attribute vec4 a_Normal;

varying vec4 v_Vertex;
varying vec2 v_Texture;
varying vec4 v_Normal;

void main()
{
  gl_Position = (u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix) * a_Vertex;
}
