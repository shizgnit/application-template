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

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
// https://stackoverflow.com/questions/48288154/pack-depth-information-in-a-rgba-texture-using-mediump-precison

float UnpackDepth16( in vec2 pack )
{
    float depth = dot( pack, 1.0 / vec2(1.0, 256.0) );
    return depth * (256.0*256.0) / (256.0*256.0 - 1.0);
}

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

   float closestDepth = texture(u_ShadowTextureUnit, Projection.xy).r;
   float currentDepth = v_Lighting.z;

   //float shadow = closestDepth;
   //float shadow = currentDepth;

   float shadow = currentDepth > closestDepth ? 1.0 : 0.5;
   //if()

   //float shadow = texture(u_ShadowTextureUnit, Projection.xy).r * 42.0f;

   //float shadow = v_Lighting.w;

   //float shadow = texture(u_ShadowTextureUnit, Projection.xy).r;
   //float shadow = Projection.z;

   //float shadow = texture(u_ShadowTextureUnit, Projection.xy).r >= 1.0 ? 1.0 : 0.5;
   //float shadow = Projection.z >= 1.0 ? 1.0 : 0.5;

   //float shadow = currentDepth > closestDepth  ? 1.5 : 0.5;

   //float shadow = v_Lighting.w == 1.0 ? 0.5 : 1.5;

   diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * vec4(vec3(shadow), 1.0f);

   // float shadow = 0.0f;
   // vec2 texelSize = vec2(1.0) / vec2(textureSize(u_ShadowTextureUnit, 0));
   // for(int x = -1; x <= 1; ++x)
   // {
   //    for(int y = -1; y <= 1; ++y)
   //    {
   //       float depth = texture(u_ShadowTextureUnit, Projection.xy + vec2(x, y) * texelSize).z;
   //       shadow += depth > 0.0 ? 1.0 : 0.0;
   //    }
   // }
   // shadow /= 9.0;

   // Light = clamp(Light / shadow, 0.0, 1.0);
  
   // diffuseColor = texture(u_SurfaceTextureUnit, v_Texture) * vec4(texture(u_SurfaceTextureUnit, v_Texture).a) * Light;
}
