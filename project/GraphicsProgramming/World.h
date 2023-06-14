#pragma once
#include <glm/glm.hpp>

class World
{

public:

	World();

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 lightDirection;
	glm::vec3 cameraPosition;
	glm::vec3 sunColor;

};

