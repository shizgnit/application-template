precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

varying vec4 v_Vertex;
varying vec4 v_Texture;
varying vec4 v_Normal;
varying vec4 v_Clipping;

void main()
{
  gl_FragColor = vec4(texture2D(u_ShadowTextureUnit, v_Texture.xy).r);
}
