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

void main(void) {
	//TODO: fix normals


	vec3 P = ex_objPosition.xyz;
	//vec3 N = tex2D(normalmap, texCoord).xyz;
	vec3 N = vec3(0.0f, 1.0f, 0.0f);
	
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
