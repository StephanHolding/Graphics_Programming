#pragma once
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include "World.h"

class Skybox
{

public:

	Skybox(GLuint programID);
	Skybox(GLuint programID, const std::vector<std::string>& cubemapPaths);

	void RenderSkybox(World& world);

private:

	void CreateSkyboxGeometry();

	GLuint programID;
	GLuint cubemap;
	GLuint skyboxVAO;
	GLuint skyboxEBO;
	int skyboxMemorySize;
	int skyboxNumofIndeces;
};

