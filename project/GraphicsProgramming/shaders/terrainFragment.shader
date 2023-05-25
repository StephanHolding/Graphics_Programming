#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;
uniform sampler2D dirtTex, sandTex, grassTex, rockTex, snowTex;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec3 lerp3(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	//normal
	vec3 normal = texture(normalTex, uv).rgb;
	normal.gb = normal.bg;
	normal = normalize(normal * 2.0 - 1.0);
	normal.r = -normal.r;
	normal.b = -normal.b;
	
	//calculate values
	float lightValue = max(-dot(normal, lightDirection), 0.0);
	float distanceToCam = length(worldPosition.xyz - cameraPosition);

	//TODO: blend textures by Y;
	float y = worldPosition.y;
	int yThreshold = 50;
	int transitionArea = 40;

	float dirtToSand = clamp((y - yThreshold) / 10, -1, 1) * 0.5 + 0.5;
	yThreshold += transitionArea;
	float sandToGrass = clamp((y - yThreshold) / 10, -1, 1) * 0.5 + 0.5;
	yThreshold += transitionArea;
	float grassToRock = clamp((y - yThreshold) / 10, -1, 1) * 0.5 + 0.5;
	yThreshold += transitionArea;
	float rockToSnow = clamp((y - yThreshold) / 10, -1, 1) * 0.5 + 0.5;

	vec3 dirtColor = texture(dirtTex, uv * 10).rgb;
	vec3 sandColor = texture(sandTex, uv * 10).rgb;
	vec3 grassColor = texture(grassTex, uv * 10).rgb;
	vec3 rockColor = texture(rockTex, uv * 10).rgb;
	vec3 snowColor = texture(snowTex, uv * 10).rgb;

	vec3 diffuse = lerp3(lerp3(lerp3(lerp3(dirtColor, sandColor, dirtToSand), grassColor, sandToGrass), rockColor, grassToRock), snowColor, rockToSnow);

	vec3 viewDirection = normalize(cameraPosition - worldPosition);
	float fog = pow(clamp((distanceToCam - 250) / 1000, 0, 1), 2);
	vec3 skyTopColor = vec3(68.0 / 255.0, 118.0 / 255.0, 189.0 / 255.0);
	vec3 skyBottomColor = vec3(188.0 / 255.0, 214.0 / 255.0, 231.0 / 255.0);
	vec3 fogColor = lerp3(skyBottomColor, skyTopColor, max(viewDirection.y, 0.0));

	//seperate RGB for A
	vec4 shaderOutput = vec4( lerp3(diffuse * min(lightValue + 0.2, 1.0), fogColor, fog), 1.0);

	FragColor = shaderOutput;
}