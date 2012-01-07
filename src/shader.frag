#version 330

uniform sampler2D			colourMap;

smooth in vec2				ex_TexCoords;

out vec4					out_Color;

void main(void) {
	//TODO: fix normals

	out_Color.rgb = texture2D(colourMap, ex_TexCoords).rgb;
}