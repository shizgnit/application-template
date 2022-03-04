#version 320 es

precision mediump float;
precision mediump int;

uniform uint u_Flags;

layout(location = 0) out vec4 diffuseColor;

void main()
{
  if(u_Flags >= uint(0x01)) {
    diffuseColor = vec4(vec3(0.0), 1.0);
  }
  else {
    diffuseColor = vec4(vec3(1.0 - gl_FragCoord.z), 1.0);
  }
}
