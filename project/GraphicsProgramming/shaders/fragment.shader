#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in mat3 TBN;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;

void main()
{
	//normal
	vec3 normal = texture(normalTex, uv).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = TBN * normal;

	//specular data
	vec3 viewDirection = normalize(worldPosition - cameraPosition);
	vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
	
	//calculate values
	float lightValue = max(-dot(normal, lightDirection), 0.0);
	float specularValue = pow(max(-dot(reflectionDirection, viewDirection), 0.0), 6);

	//seperate RGB for A
	vec4 shaderOutput = vec4(color, 1.0f) * texture(mainTex, uv);
	shaderOutput.rgb = shaderOutput.rgb * min(lightValue + 0.2, 1.0) + specularValue;

	//FragColor = vec4(color, 1.0f) * texture(normalTex, uv); //for testing specific textures
	FragColor = shaderOutput;
}