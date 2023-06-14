#include "Actor.h"

Actor::Actor(const std::string& name)
{
	SetActorName(name);
	InitializeWithStandardTransform();
}

void Actor::SetActorName(const std::string& name)
{
	actorName = name;
}

const std::string& Actor::GetActorName()
{
	return actorName;
}

void Actor::InitializeWithStandardTransform()
{
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
}
