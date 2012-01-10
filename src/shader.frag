#version 330

uniform sampler2D			colourMap;
uniform sampler2D			colourMap2;
uniform float				interpolationScale;

smooth in vec2				ex_TexCoords;

out vec4					out_Color;

void main(void) {
	//TODO: fix normals

	vec3 color1 = texture2D(colourMap, ex_TexCoords).rgb;
	vec3 color2 = texture2D(colourMap2, ex_TexCoords).rgb;

	out_Color.rgb = mix(color1, color2, interpolationScale).rgb;
}