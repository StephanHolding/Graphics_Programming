#pragma once

#include "Actor.h"
#include "Material.h"
#include "model.h"
#include "World.h"

class ModelActor : public Actor
{

public:

	ModelActor(const std::string& name, const char* modelPath, GLuint programID, Material* material);
	~ModelActor();

	void Render(World& world) const;
	void SetModel(const char* modelPath, GLuint programID, Material* material);

private:
	Model* model = nullptr;
	GLuint programID = 0;
	Material* material = nullptr;
};

