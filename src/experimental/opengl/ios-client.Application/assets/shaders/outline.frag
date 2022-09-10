#version 320 es

precision mediump float;
precision mediump int;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_NormalTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

uniform uint u_Flags;

in vec2 v_Texture;
in vec4 v_Vertex;
in vec4 v_Normal;
in vec4 v_Lighting;

in float v_FrontFacing;

layout(location = 0) out vec4 diffuseColor;

void main()
{
   vec4 L = normalize(u_AmbientLightPosition - v_Vertex);
   vec4 E = normalize(-v_Vertex);
   vec4 R = normalize(-reflect(L,v_Normal));
   
   float d = max(dot(v_Normal, L), 0.0);

   if(v_FrontFacing <= 0.0) {
      discard;
   }

   diffuseColor = vec4(0.0, 0.0, 0.0, 1.0);
}