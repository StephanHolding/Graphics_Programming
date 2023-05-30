#version 330 core
out vec4 FragColor;

in vec4 worldPosition;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

uniform samplerCube cubeMap;

vec3 lerp3(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec3 sunColor = vec3(0.8, 0.8, 1.0) * 1.5;

	//TODO: WAAROM HEB IK 2 ZONNEN?

	//specular data
	vec3 viewDirection = normalize(worldPosition.rgb - cameraPosition);

	//calculate sun
	float sun = max(pow(dot(-viewDirection, lightDirection), 128), 0.0);

	vec4 cubeMapColor = texture(cubeMap, viewDirection);

	vec4 shaderOutput = cubeMapColor + vec4(sun * sunColor, 1);
	FragColor = shaderOutput;
}