#type vertex
#version 410 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aTilingFactor;

uniform mat4 u_ViewProjection; 

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;

void main()
{ 
	v_TexCoord = aTexCoord;
  v_Color = aColor;
  v_TexIndex = aTexIndex;
  v_TilingFactor = aTilingFactor;
  gl_Position = u_ViewProjection * vec4(aPos, 1.0); 
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

//Bunch of textures storable
uniform sampler2D u_Textures[16]; //Uniform for the texture we will set from the application code

void main()
{
  //Outputing as color, first 2 components become the colro and x and y are red and green and 0 blue and 1 alpha
  //Visulaisation of data we put in vertex buffer as texture coordinates
  //FragColor = vec4(v_TexCoord, 0.0, 1.0);
  //FragColor = texture(u_Texture, v_TexCoord * 100.0) * u_Color;//Sample the texture with the texture coordinates and output it as the fragment color
  //float dist = 1.0f - distance(v_ScreenPos * 0.8f, vec2(0.0f));
	//dist = clamp(dist, 0.0f, 1.0f);
	//dist = sqrt(dist);
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color; //* dist
}