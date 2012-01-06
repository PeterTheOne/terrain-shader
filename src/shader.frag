#version 330

uniform sampler2D			colourMap;

in vec4						ex_Color;
smooth in vec2				ex_TexCoords;

out vec4					out_Color;

void main(void)
{
	out_Color.rgb = texture2D(colourMap, ex_TexCoords).rgb;
}
