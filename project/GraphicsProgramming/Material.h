#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>


class Material
{
public:
	bool useAlpha;
	glm::vec3 diffuseColor;
	float normalStrength;

	Material(const char* diffusePath, const char* normalPath, const char* specularPath, const char* roughnessPath, const char* aoPath);
	void BindTextures() const;

private:

	std::vector<GLuint> textureIDs;
	GLuint emptyTex;

	GLuint EmptyPathCheck(const char* path) const;
};

