#version 330

uniform mat4				ModelMatrix;
uniform mat4				ViewMatrix;
uniform mat4				ProjectionMatrix;
uniform sampler2D			heightMap;
uniform sampler2D			heightMap2;
uniform float				terrainScale;
uniform float				interpolationScale;

layout(location=0) in vec4	in_Position;
layout(location=1) in vec2	in_Texture;

out vec3					ex_objPosition;
smooth out vec2				ex_TexCoords;

void main(void) {
	vec4 texColor1 = texture2D(heightMap, in_Texture);
	vec4 texColor2 = texture2D(heightMap2, in_Texture);
	vec4 texColor = mix(texColor1, texColor2, interpolationScale);

	// TODO: repalce texColor.r with texColor.a for alpha?
	vec4 position = vec4(in_Position.x, texColor.r * terrainScale, in_Position.z, 1.0f);

	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * position;
	
	ex_objPosition = in_Position.xyz;

	// Pass the texture coordinates through the vertex shader so they get smoothly interpolated
    ex_TexCoords = in_Texture.st;
}
