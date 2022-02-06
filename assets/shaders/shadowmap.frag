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

vec2 PackDepth16( in float depth )
{
    float depthVal = depth * (256.0*256.0 - 1.0) / (256.0*256.0);
    vec3 encode = fract( depthVal * vec3(1.0, 256.0, 256.0*256.0) );
    return encode.xy - encode.yz / 256.0 + 1.0/512.0;
}

layout(location = 0) out vec4 diffuseColor;

void main()
{
    vec3 Projection = v_Lighting.xyz / v_Lighting.w;
    Projection = Projection * 0.5 + 0.5;

    diffuseColor = vec4(Projection, 1.0);
}