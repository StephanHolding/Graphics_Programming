#pragma once
#include <glm/glm.hpp>
#include <iostream>

class Actor
{

public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Actor(const std::string& name);

	void SetActorName(const std::string& name);
	const std::string& GetActorName();

protected:
	std::string actorName;

	void InitializeWithStandardTransform();
};

