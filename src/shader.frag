#version 330

uniform sampler2D			heightMap;
uniform sampler2D			normalMap;

// light
uniform	vec3				Ka;
uniform vec3				Kd;
uniform vec3				globalAmbient;
uniform	vec3				lightColor;
uniform vec3				lightPosition;

out vec3					ex_objPosition;
smooth in vec2				ex_TexCoords;

out vec4					out_Color;

void main(void) {
	//TODO: fix normals


	vec3 P = ex_objPosition.xyz;
	vec3 N = texture2D(normalMap, ex_TexCoords).xyz;
	
	// Compute ambient term
	vec3 ambient = Ka * globalAmbient;

	// Compute the diffuse term
	vec3 L = normalize(lightPosition - P);
	float diffuseLight = max(dot(L, N), 0);
	vec3 diffuse = Kd * lightColor * diffuseLight;

	out_Color.rgb = (ambient + diffuse);
}
