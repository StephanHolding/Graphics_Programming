#pragma once
#include <glad/glad.h>

class Shaders
{

public:
	Shaders();

	//Program IDs
	GLuint simpleProgram;
	GLuint skyboxProgram;
	GLuint terrainProgram;
	GLuint modelProgram;
	GLuint starProgram;
	GLuint planetProgram;
	GLuint blitProgram;
	GLuint inverseImageProgram;
	GLuint hdrShader;

private:
	void CreateShaders();
	GLuint CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
	void CheckShaderCompileStatus(const GLuint& id);
	char* ReadFile(const char* filename);
};

