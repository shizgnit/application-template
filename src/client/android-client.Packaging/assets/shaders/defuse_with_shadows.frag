precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

varying vec2 v_Texture;
varying vec4 v_Vertex;
varying vec4 v_Normal;
varying vec4 v_Lighting;

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer

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

   //float depth = texture2D(u_ShadowTextureUnit, Projection.xy).r;
   //float shadow = depth != 1.0 ? 0.5 : 1.0;
   //float shadow = Projection.z >= 10.0 ? 0.5 : 1.0;

   float shadow = 0.0;
   vec2 texelSize = vec2(1.0 / 1024.0f);
   //vec2 texelSize = 1.0 / textureSize(u_ShadowTextureUnit, 0);   
   for(int x = -1; x <= 1; ++x)
   {
      for(int y = -1; y <= 1; ++y)
      {
         float depth = texture2D(u_ShadowTextureUnit, Projection.xy + vec2(x, y) * texelSize).r; 
         shadow += depth != 1.0 ? 1.0 : 0.0;
      }
   }
   shadow /= 9.0;

   Light = clamp(Light / shadow, 0.0, 1.0);
  
   gl_FragColor = texture2D(u_SurfaceTextureUnit, v_Texture) * vec4(texture2D(u_SurfaceTextureUnit, v_Texture).a) * Light;
}