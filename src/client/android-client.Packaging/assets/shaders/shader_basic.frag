precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLight;
uniform vec4 u_DirectionalLight;
uniform vec4 u_DirectionalLightPosition;

varying vec2 v_Texture;
varying vec4 v_Vertex;
varying vec4 v_Normal;
varying vec4 v_Clipping;

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

  gl_FragColor = gl_FragColor + texture2D(u_SurfaceTextureUnit, v_Texture);
}
