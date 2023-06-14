#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;
in mat3 TBN;

uniform vec3 color_diffuse;
uniform sampler2D texture_diffuse;

uniform float normal_strength;
uniform sampler2D texture_normal;

uniform sampler2D texture_specular;
uniform sampler2D texture_roughness;
uniform sampler2D texture_ao;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;
uniform vec3 sunColor;

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

vec4 lerp(vec4 a, vec4 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec4 diffuse = texture(texture_diffuse, TexCoords) * vec4(color_diffuse, 1.0f);
	vec3 normal = texture(texture_normal, TexCoords).rgb;
	vec4 specTex = texture(texture_specular, TexCoords);
	float ambientOcclusion = texture(texture_ao, TexCoords).r;
	float roughness = texture(texture_roughness, TexCoords).r;

	//calculate Normal Map
	normal = normalize(normal * 2.0 - 1.0);
	normal = TBN * normal;
	normal = normal * normal_strength;

	float light = max(dot(-lightDirection, normal), 0.0);

	vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
	vec3 refl = reflect(lightDirection, normal);

	float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 128, roughness));
	vec3 specular = spec * specTex.r * sunColor;

	float distanceToCam = length(FragPos.xyz - cameraPosition);
	float fog = pow(clamp((distanceToCam - 250) / 1000, 0, 1), 2);
	vec3 skyTopColor = vec3(68.0 / 255.0, 118.0 / 255.0, 189.0 / 255.0);
	vec3 skyBottomColor = vec3(188.0 / 255.0, 214.0 / 255.0, 231.0 / 255.0);
	vec3 fogColor = lerp(skyBottomColor * sunColor, skyTopColor * sunColor, max(viewDir.y, 0.0));

	vec4 fragOutput = lerp(diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) * vec4(sunColor, 1.0) + vec4(specular, 0), vec4(fogColor, 1.0), fog);
	FragColor = fragOutput;
}