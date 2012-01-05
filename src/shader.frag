#version 330

in vec4 ex_Color;
smooth in vec2 ex_TexCoords;

out vec4 out_Color;

uniform sampler2D colourMap; // This is the original texture

void main(void)
{
	out_Color.rgb = texture2D(colourMap, ex_TexCoords).rgb;
}
