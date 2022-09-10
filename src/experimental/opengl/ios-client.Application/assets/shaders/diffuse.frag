#version 320 es

precision mediump float;
precision mediump int;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_NormalTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec2 v_Texture;
in vec4 v_Vertex;
in vec4 v_Normal;
in vec4 v_Lighting;

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer

layout(location = 0) out vec4 diffuseColor;

void main()
{
   vec4 L = normalize(u_AmbientLightPosition - v_Vertex);
   vec4 E = normalize(-v_Vertex);
   vec4 R = normalize(-reflect(L,v_Normal));
   
   float d = 0.6 * max(dot(v_Normal, L), 0.0);
   vec4 Idiff = vec4(d, d, d, d);
   
   vec4 Light = vec4(Idiff.xyz, 1.0) + u_AmbientLightColor;

   Light = clamp(Light, 0.0, 1.0);
  
   diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * Light;
}
