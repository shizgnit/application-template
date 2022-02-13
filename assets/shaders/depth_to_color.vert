uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_Clipping;

uniform vec4 u_AmbientLightPosition;
uniform vec4 u_AmbientLightColor;

in mat4 a_ModelMatrix;

attribute vec4 a_Vertex;
attribute vec4 a_Texture;
attribute vec4 a_Normal;

varying vec4 v_Vertex;
varying vec4 v_Texture;
varying vec4 v_Normal;
varying vec4 v_Clipping;

void main()
{
  mat4 MVP = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix;

  v_Texture = a_Texture;
  v_Vertex = MVP * a_Vertex;
  v_Normal = normalize(MVP * a_Normal);
  
  gl_Position = v_Vertex;

  vec4 model = u_ModelMatrix * a_Vertex;

  float top = dot( model, vec4( 0.0, -1.0, 0.0, u_Clipping.x ));
  float bottom = dot( model, vec4( 0.0, 1.0, 0.0, u_Clipping.y ));
  float left = dot( model, vec4( 1.0, 0.0, 0.0, u_Clipping.z ));
  float right = dot( model, vec4( -1.0, 0.0, 0.0, u_Clipping.w ));

  v_Clipping = vec4(top, bottom, left, right);
}
