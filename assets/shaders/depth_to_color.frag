#version 320 es

precision mediump float;
precision mediump int;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_NormalTextureUnit;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec4 v_Vertex;
in vec2 v_Texture;
in vec4 v_Normal;
in vec4 v_Clipping;

layout(location = 0) out vec4 diffuseColor;

void main()
{
  diffuseColor = vec4(texture(u_ShadowTextureUnit, v_Texture.xy).r);
}
