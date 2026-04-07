#type vertex
#version 410 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main()
{ 
  v_TexCoord = aTexCoord;
  gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
}

#type fragment
#version 410 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture; //Uniform for the texture we will set from the application code

void main()
{
  //Outputing as color, first 2 components become the colro and x and y are red and green and 0 blue and 1 alpha
  //Visulaisation of data we put in vertex buffer as texture coordinates
  //FragColor = vec4(v_TexCoord, 0.0, 1.0);
  FragColor = texture(u_Texture, v_TexCoord); //Sample the texture with the texture coordinates and output it as the fragment color
}