#version 320 es

precision mediump float;

layout(location = 0) out vec4 diffuseColor;

void main()
{
  diffuseColor = vec4(vec3(1.0 - gl_FragCoord.z), 1.0);
}
