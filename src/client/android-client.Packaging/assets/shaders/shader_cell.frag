uniform sampler2D u_SurfaceTextureUnit;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLight;
uniform vec4 u_DirectionalLight;
uniform vec4 u_DirectionalLightPosition;

varying vec4 v_Vertex;
varying vec4 v_Texture;
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

  vec4 n = normalize(v_Normal);
  float i = dot(vec4(1.0, 1.0, 1.0, 1.0), n);

  vec4 color;
	if (i > 0.95)
		color = vec4(1.0,0.0,0.0,1.0);
	else if (i > 0.5)
		color = vec4(0.0,1.0,0.0,1.0);
	else if (i > 0.25)
		color = vec4(0.0,0.0,1.0,1.0);
	else
		color = vec4(1.0,0.0,0.0,1.0);

// up - 38
// down - 40
// left - 37
// right - 39
// end - 35
// home - 36

//	if (intensity > 0.95)
//		color = vec4(1.0,0.5,0.5,1.0);
//	else if (intensity > 0.5)
//		color = vec4(0.6,0.3,0.3,1.0);
//	else if (intensity > 0.25)
//		color = vec4(0.4,0.2,0.2,1.0);
//	else
//		color = vec4(0.2,0.1,0.1,1.0);

  gl_FragColor = color; // texture2D(u_SurfaceTextureUnit, v_Texture.xy) + color;
}
