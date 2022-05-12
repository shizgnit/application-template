#version 320 es

precision mediump float;
precision mediump int;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_NormalTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;
uniform float u_AmbientLightBias;
uniform float u_AmbientLightStrength;

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
   
   vec4 Idiff = vec4(u_AmbientLightStrength * max(dot(v_Normal, L), 0.0));
   vec4 Light = vec4(Idiff.xyz, 1.0) + u_AmbientLightColor;

   vec3 Projection = v_Lighting.xyz / v_Lighting.w;
   Projection = Projection * 0.5 + 0.5;

   float bias = u_AmbientLightBias; //max(u_AmbientLightBias * (1.0 - dot(L, v_Normal)), u_AmbientLightBias / 10.0f);
   float currentDepth = Projection.z;

   //float closestDepth = texture(u_ShadowTextureUnit, Projection.xy).r;
   //float shadow = currentDepth > closestDepth ? 0.5 : 1.0;

   //diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * vec4(vec3(shadow), 1.0f);

   float shadow = 0.0f;
   vec2 texelSize = vec2(1.0) / vec2(textureSize(u_ShadowTextureUnit, 0));
   for(int x = -1; x <= 1; ++x)
   {
       for(int y = -1; y <= 1; ++y)
       {
          float shadowDepth = texture(u_ShadowTextureUnit, Projection.xy + vec2(x, y) * texelSize).r + bias;
          shadow += currentDepth > shadowDepth ? u_AmbientLightStrength : 1.0;
       }
   }
   shadow /= 9.0;

   Light = clamp(Light * shadow, 0.0, 1.0);

   diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * vec4(vec3(Light), 1.0f);
}