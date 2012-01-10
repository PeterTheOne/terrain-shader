#version 330

uniform mat4				ModelMatrix;
uniform mat4				ViewMatrix;
uniform mat4				ProjectionMatrix;
uniform sampler2D			heightMap;
uniform float				terrainScale;

layout(location=0) in vec4	in_Position;
layout(location=1) in vec2	in_Texture;

out vec3					ex_objPosition;
smooth out vec2				ex_TexCoords;

void main(void) {
	//TODO: fix normals

	vec4 texColor = texture2D(heightMap, in_Texture);
	// TODO: repalce texColor.r with texColor.a for alpha?
	vec4 position = vec4(in_Position.x, texColor.r * terrainScale, in_Position.z, 1.0f);

	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * position;
	
	ex_objPosition = in_Position.xyz;

	// Pass the texture coordinates through the vertex shader so they get smoothly interpolated
    ex_TexCoords = in_Texture.st;
}