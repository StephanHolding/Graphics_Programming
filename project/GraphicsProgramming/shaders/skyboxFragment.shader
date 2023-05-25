#version 330 core
out vec4 FragColor;

in vec4 worldPosition;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec3 lerp3(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec3 skyTopColor = vec3(68.0 / 255.0, 118.0 / 255.0, 189.0 / 255.0);
	vec3 skyBottomColor = vec3(188.0 / 255.0, 214.0 / 255.0, 231.0 / 255.0);

	vec3 sunColor = vec3(1.0, 200.0 / 255.0, 50.0 / 255.0);


	//specular data
	vec3 viewDirection = normalize(worldPosition.rgb - cameraPosition);

	//calculate sun
	float sun = max(pow(dot(-viewDirection, lightDirection), 128), 0.0);

	vec4 shaderOutput = vec4(lerp3(skyBottomColor, skyTopColor, abs(viewDirection.y)) + sun * sunColor, 1);
	FragColor = shaderOutput;
}