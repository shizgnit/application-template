#version 320 es

precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec2 v_Texture;
in vec4 v_Vertex;
in vec4 v_Normal;
in vec4 v_Lighting;

layout(location = 0) out vec4 diffuseColor;

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
  
   diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * Light;
}