#include "Shaders.h"
#include <fstream>
#include <iostream>

Shaders::Shaders()
{
	CreateShaders();
}

void Shaders::CreateShaders()
{
	simpleProgram = CreateProgram("shaders/vertex.shader", "shaders/fragment.shader");

	glUseProgram(simpleProgram);
	glUniform1i(glGetUniformLocation(simpleProgram, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(simpleProgram, "normalTex"), 1);

	skyboxProgram = CreateProgram("shaders/skyboxVertex.shader", "shaders/skyboxFragment.shader");



	terrainProgram = CreateProgram("shaders/terrainVertex.shader", "shaders/terrainFragment.shader");
	glUseProgram(terrainProgram);
	glUniform1i(glGetUniformLocation(terrainProgram, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(terrainProgram, "normalTex"), 1);
	glUniform1i(glGetUniformLocation(terrainProgram, "dirtTex"), 2);
	glUniform1i(glGetUniformLocation(terrainProgram, "sandTex"), 3);
	glUniform1i(glGetUniformLocation(terrainProgram, "grassTex"), 4);
	glUniform1i(glGetUniformLocation(terrainProgram, "rockTex"), 5);
	glUniform1i(glGetUniformLocation(terrainProgram, "snowTex"), 6);

	modelProgram = CreateProgram("shaders/modelVertex.shader", "shaders/modelFragment.shader");
	glUseProgram(modelProgram);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_diffuse"), 0);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_normal"), 1);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_specular"), 2);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_roughness"), 3);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_ao"), 4);

	starProgram = CreateProgram("shaders/skyboxVertex.shader", "shaders/starFragment.shader");

	planetProgram = CreateProgram("shaders/modelVertex.shader", "shaders/planetFragment.shader");
	glUseProgram(planetProgram);
	glUniform1i(glGetUniformLocation(planetProgram, "day"), 0);
	glUniform1i(glGetUniformLocation(planetProgram, "night"), 1);
	glUniform1i(glGetUniformLocation(planetProgram, "clouds"), 2);

	blitProgram = CreateProgram("shaders/imageVertex.shader", "shaders/imageFragment.shader");
	inverseImageProgram = CreateProgram("shaders/imageVertex.shader", "shaders/inverseImageFragment.shader");
	hdrShader = CreateProgram("shaders/imageVertex.shader", "shaders/hdrFragment.shader");
}

GLuint Shaders::CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	const char* vertexSrc = ReadFile(vertexShaderPath);
	const char* fragmentSrc = ReadFile(fragmentShaderPath);

	const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShaderID);

	CheckShaderCompileStatus(vertexShaderID);

	const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSrc, nullptr);
	glCompileShader(fragmentShaderID);

	CheckShaderCompileStatus(fragmentShaderID);

	const GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	int linkSuccess;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(programID, 512, nullptr, infoLog);
		std::cout << "ERROR LINKING PROGRAM: \n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	delete vertexSrc;
	delete fragmentSrc;

	return programID;
}

void Shaders::CheckShaderCompileStatus(const GLuint& id)
{
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		char infoLog[512];
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cout << "ERROR COMPILING SHADER: \n" << infoLog << std::endl;
	}
}

char* Shaders::ReadFile(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.is_open())
	{
		file.seekg(0, file.end);
		const int fileLength = file.tellg();
		file.seekg(0, file.beg);

		char* output = new char[fileLength + 1];
		file.read(output, fileLength);
		output[fileLength] = '\0';

		file.close();

		return output;
	}
	else
	{
		return nullptr;
	}
}
