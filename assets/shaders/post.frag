#version 320 es

precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_NormalTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_DepthTextureUnit;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec4 v_Vertex;
in vec4 v_Texture;
in vec4 v_Normal;

layout(location = 0) out vec4 diffuseColor;

void main()
{
  float dx = 1.0 / float(textureSize(u_DepthTextureUnit, 0).x);
  float dy = 1.0 / float(textureSize(u_DepthTextureUnit, 0).y);
  
  float s00 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(-dx,dy) ).r;
  float s10 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(-dx,0.0) ).r;
  float s20 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(-dx,-dy) ).r;
  
  float s01 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(0.0,dy) ).r;
  float s21 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(0.0,-dy) ).r;
  float s02 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(dx,dy) ).r;
  
  float s12 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(dx,0.0) ).r;
  float s22 = texture( u_DepthTextureUnit, v_Texture.xy + vec2(dx,-dy) ).r;
  
  float sx = s00 + 2.0 * s10 + s20 - (s02 + 2.0 * s12 + s22);
  float sy = s00 + 2.0 * s01 + s02 - (s20 + 2.0 * s21 + s22);

  float length = length(vec2(sx, sy));
  float magnitude = length > 0.1 ? length : 0.0;

  diffuseColor = texture(u_SurfaceTextureUnit, v_Texture.xy) - magnitude;
}
