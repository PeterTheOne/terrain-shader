#version 330

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
layout(location=2) in vec4 in_Texture;
out vec4 ex_Color;
smooth out vec2 ex_TexCoords;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
	ex_Color = in_Color;
	
	// Pass the texture coordinates through the vertex shader so they get smoothly interpolated
    ex_TexCoords = in_Texture.st;
}
