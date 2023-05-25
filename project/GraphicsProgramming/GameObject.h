#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include "model.h"

class GameObject
{

public:
	GameObject(std::string objectName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	void PrepareRendering(Model* model, GLuint programID);
	void Render();

private:
	
	std::string objectName;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Model* toRender;
	GLuint programID;

};

