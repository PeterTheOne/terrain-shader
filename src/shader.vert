#version 330

uniform mat4				ModelMatrix;
uniform mat4				ViewMatrix;
uniform mat4				ProjectionMatrix;

layout(location=0) in vec4	in_Position;
layout(location=1) in vec2	in_Texture;

smooth out vec2				ex_TexCoords;

void main(void)
{
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
	
	// Pass the texture coordinates through the vertex shader so they get smoothly interpolated
    ex_TexCoords = in_Texture.st;
}
