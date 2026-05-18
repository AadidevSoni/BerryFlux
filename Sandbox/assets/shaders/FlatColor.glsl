#type vertex
#version 410 core
layout(location = 0) in vec3 aPos;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{ 
  gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
}

#type fragment
#version 410 core

layout(location=0) out vec4 color;

uniform vec4 u_Color; //Uniform for the texture we will set from the application code

void main()
{
  //Outputing as color, first 2 components become the colro and x and y are red and green and 0 blue and 1 alpha
  //Visulaisation of data we put in vertex buffer as texture coordinates
  //FragColor = vec4(v_TexCoord, 0.0, 1.0);
  color = u_Color; //Sample the texture with the texture coordinates and output it as the fragment color
}