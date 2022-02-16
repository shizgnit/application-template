#version 320 es

precision mediump float;

uniform sampler2D u_SurfaceTextureUnit;
uniform sampler2D u_ShadowTextureUnit;
uniform sampler2D u_NormalTextureUnit;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in vec2 v_Texture;
in vec4 v_Vertex;
in vec4 v_Normal;
in vec4 v_Lighting;

layout(location = 0) out vec4 diffuseColor;

void main()
{
    vec3 Projection = v_Lighting.xyz / v_Lighting.w;
    Projection = Projection * 0.5 + 0.5;

    diffuseColor = vec4(vec3(Projection.z), 1.0);
}