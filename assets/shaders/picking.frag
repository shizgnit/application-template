#version 320 es

precision mediump float;
precision mediump int;

uniform mat4 u_Parameters;

flat in uint v_Identifier;

layout(location = 0) out vec4 diffuseColor;

void main()
{ 
  float unit = (1.0 / 256.0);
  float upper = unit * float(v_Identifier / uint(255));
  float lower = unit * float(v_Identifier % uint(255));
  diffuseColor = vec4(u_Parameters[1][1], upper, lower, 1.0);
}