#pragma once
#include <glad/glad.h>
#include <vector>

#include "Actor.h"
#include "World.h"

class Terrain : public Actor
{
public:
	Terrain(const std::string& name, const GLuint programID, const char* heightMapPath, const char* normalMapPath, const std::vector<const char*> terrainTexturePaths);

	void RenderTerrain(World& world);

private:

	unsigned int GenerateTerrain(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);

	GLuint terrainProgram;
	GLuint terrainVAO;
	GLuint terrainIndexCount;
	GLuint heightmapID;
	GLuint heightNormalID;
	std::vector< GLuint> terrainTextureIDs;
	unsigned char* heightMapTexture;
};

