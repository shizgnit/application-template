#version 320 es

precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;
uniform float u_AmbientLightBias;

in vec2 v_Texture;
in vec4 v_Vertex;
in vec4 v_Normal;
in vec4 v_Lighting;

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
// https://stackoverflow.com/questions/48288154/pack-depth-information-in-a-rgba-texture-using-mediump-precison

layout(location = 0) out vec4 diffuseColor;

void main()
{
   vec4 L = normalize(u_AmbientLightPosition - v_Vertex);
   vec4 E = normalize(-v_Vertex);
   vec4 R = normalize(-reflect(L,v_Normal));
   
   float d = 0.6 * max(dot(v_Normal, L), 0.0);
   vec4 Idiff = vec4(d, d, d, d);
   
   vec4 Light = vec4(Idiff.xyz, 1.0) + u_AmbientLightColor;

   vec3 Projection = v_Lighting.xyz / v_Lighting.w;
   Projection = Projection * 0.5 + 0.5;

   float bias = min(u_AmbientLightBias * (1.0 - dot(v_Normal, L)), u_AmbientLightBias);
   float currentDepth = Projection.z + bias;

   //float closestDepth = texture(u_ShadowTextureUnit, Projection.xy).r;
   //float shadow = currentDepth > closestDepth ? 0.5 : 1.0;

   //diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * vec4(vec3(shadow), 1.0f);

   float shadow = 0.0f;
   vec2 texelSize = vec2(1.0) / vec2(textureSize(u_ShadowTextureUnit, 0));
   for(int x = -1; x <= 1; ++x)
   {
       for(int y = -1; y <= 1; ++y)
       {
          float shadowDepth = texture(u_ShadowTextureUnit, Projection.xy + vec2(x, y) * texelSize).r;
          shadow += currentDepth > shadowDepth ? 0.5 : 1.0;
       }
   }
   shadow /= 9.0;

   Light = clamp(Light * shadow, 0.0, 1.0);

   diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * vec4(vec3(Light), 1.0f);
}
