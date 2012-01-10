#version 330

uniform sampler2D			heightMap;
uniform float				terrainScale;

// light
uniform	vec3				Ka;
uniform vec3				Kd;
uniform vec3				globalAmbient;
uniform	vec3				lightColor;
uniform vec3				lightPosition;

out vec3					ex_objPosition;
smooth in vec2				ex_TexCoords;

out vec4					out_Color;

// Obtain fragment elevation
float getElevation(vec2 texcoord) {
    return (texture2D(heightMap, texcoord).r);
}

vec3 getNormal(vec2 texcoord, float sOffset, float tOffset) {
	vec2 texcoordN = vec2(texcoord.s,			texcoord.t + tOffset);
	vec2 texcoordE = vec2(texcoord.s + sOffset,	texcoord.t);
	vec2 texcoordS = vec2(texcoord.s,			texcoord.t - tOffset);
	vec2 texcoordW = vec2(texcoord.s - sOffset,	texcoord.t);
	
	vec3 v0 = vec3(0.0f,		getElevation(texcoord), 0.0f);

	vec3 v1 = vec3(0.0f,		getElevation(texcoordN), +tOffset)	- v0;
	vec3 v2 = vec3(+sOffset,	getElevation(texcoordE), 0.0f)		- v0;
	vec3 v3 = vec3(0.0f,		getElevation(texcoordS), -tOffset)	- v0;
	vec3 v4 = vec3(-sOffset,	getElevation(texcoordW), 0.0f)		- v0;
	
	vec3 n0 = normalize(cross(v1, v2));
	vec3 n1 = normalize(cross(v2, v3));
	vec3 n2 = normalize(cross(v3, v4));
	vec3 n3 = normalize(cross(v4, v1));

	return (n0 + n1 + n2 + n3) / 4;
}

void main(void) {
	vec3 P = ex_objPosition.xyz;
	vec3 N = getNormal(ex_TexCoords, 0.01, 0.01);
	
	// Compute ambient term
	vec3 ambient = Ka * globalAmbient;

	// Compute the diffuse term
	vec3 L = normalize(lightPosition - P);
	float diffuseLight = max(dot(L, N), 0);
	vec3 diffuse = Kd * lightColor * diffuseLight;

	out_Color.rgb = vec3(0.0f);
	out_Color.rgb = ambient + diffuse;
}
