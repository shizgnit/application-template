precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

varying vec2 v_Texture;
varying vec4 v_Vertex;
varying vec4 v_Normal;

void main()
{
   vec4 L = normalize(u_AmbientLightPosition - v_Vertex);
   vec4 E = normalize(-v_Vertex);
   vec4 R = normalize(-reflect(L,v_Normal));
   
   float d = max(dot(v_Normal, L), 0.0);
   float c = 0.4;
   
   if (d > 0.95)
	 c = 1.2;
   else if (d > 0.5)
     c = 1.0;
   else if (d > 0.25)
     c = 0.5;
   
   vec4 Light = vec4(c, c, c, 1.0);
  
   gl_FragColor = texture2D(u_SurfaceTextureUnit, v_Texture) * vec4(texture2D(u_SurfaceTextureUnit, v_Texture).a) * Light;
}