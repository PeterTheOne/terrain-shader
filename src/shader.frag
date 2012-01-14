#version 330

uniform sampler2D			heightMap;
uniform sampler2D			grasTexture;
uniform sampler2D			stoneTexture;
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

const float					normalContrastScale = 0.3f;

// Obtain fragment elevation
float getElevation(vec2 texcoord) {
    return (texture2D(heightMap, texcoord).r * normalContrastScale);
}

vec3 getNormal(vec2 texcoord, float sOffset, float tOffset) {
	vec2 texcoordN =	vec2(texcoord.s,			texcoord.t + tOffset);
	vec2 texcoordNE =	vec2(texcoord.s + sOffset,	texcoord.t + tOffset);
	vec2 texcoordE =	vec2(texcoord.s + sOffset,	texcoord.t);
	vec2 texcoordSE =	vec2(texcoord.s + sOffset,	texcoord.t - tOffset);
	vec2 texcoordS =	vec2(texcoord.s,			texcoord.t - tOffset);
	vec2 texcoordSW =	vec2(texcoord.s - sOffset,	texcoord.t - tOffset);
	vec2 texcoordW =	vec2(texcoord.s - sOffset,	texcoord.t);
	vec2 texcoordNW =	vec2(texcoord.s - sOffset,	texcoord.t + tOffset);
	
	vec3 v[9];
	v[0] = vec3(0.0f,		getElevation(texcoord),		0.0f);
	v[1] = vec3(0.0f,		getElevation(texcoordN),	+tOffset)	- v[0];
	v[2] = vec3(+sOffset,	getElevation(texcoordNE),	+tOffset)	- v[0];
	v[3] = vec3(+sOffset,	getElevation(texcoordE),	0.0f)		- v[0];
	v[4] = vec3(+sOffset,	getElevation(texcoordSE),	-tOffset)	- v[0];
	v[5] = vec3(0.0f,		getElevation(texcoordS),	-tOffset)	- v[0];
	v[6] = vec3(-tOffset,	getElevation(texcoordSW),	-tOffset)	- v[0];
	v[7] = vec3(-sOffset,	getElevation(texcoordW),	0.0f)		- v[0];
	v[8] = vec3(-sOffset,	getElevation(texcoordNW),	+tOffset)	- v[0];
	
	vec3 n0 = normalize(cross(v[1], v[2]));
	vec3 n1 = normalize(cross(v[2], v[3]));
	vec3 n2 = normalize(cross(v[3], v[4]));
	vec3 n3 = normalize(cross(v[4], v[5]));
	vec3 n4 = normalize(cross(v[5], v[6]));
	vec3 n5 = normalize(cross(v[6], v[7]));
	vec3 n6 = normalize(cross(v[7], v[8]));
	vec3 n7 = normalize(cross(v[8], v[1]));
	
	return normalize(n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7);
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

	// Height Texture Blending
	float textureScale = 10;
	vec3 grasTexture = texture2D(grasTexture, mod(ex_TexCoords * textureScale, 1)).rgb;
	vec3 stoneTexture = texture2D(stoneTexture, mod(ex_TexCoords * textureScale, 1)).rgb;

	float height = texture2D(heightMap, ex_TexCoords).r;

	vec3 blendedTexture;
	if (height > 0.6) {
		blendedTexture = mix(grasTexture, stoneTexture, (height - 0.6) / 4 * 10);
	} else {
		blendedTexture = grasTexture;
	}

	out_Color.rgb = (ambient + diffuse) * blendedTexture;
}
