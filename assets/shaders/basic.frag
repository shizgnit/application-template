#version 320 es

precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec4 v_Vertex;
in vec4 v_Texture;
in vec4 v_Normal;
in vec4 v_Clipping;

layout(location = 0) out vec4 diffuseColor;

void main()
{
  if ( v_Clipping.x < 0.0 )
    discard;

  if ( v_Clipping.y < 0.0 )
    discard;

  if ( v_Clipping.z < 0.0 )
    discard;

  if ( v_Clipping.w < 0.0 )
    discard;

  diffuseColor = texture(u_SurfaceTextureUnit, v_Texture.xy);
}
