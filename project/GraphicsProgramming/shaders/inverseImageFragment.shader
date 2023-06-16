#version 330 core

in vec2 fragCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	FragColor = vec4(1 - texture(screenTexture, fragCoords));
}