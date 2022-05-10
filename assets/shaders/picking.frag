#version 320 es

precision mediump float;
precision mediump int;

uniform mat4 u_Parameters;

layout(location = 0) out vec4 diffuseColor;

void main()
{  
  diffuseColor = vec4(u_Parameters[1][1], 0.0, 0.0, 1.0);
}