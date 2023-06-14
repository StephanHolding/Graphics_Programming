#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Material.h"
#include "Loader.h"
#include "ModelActor.h"
#include "Skybox.h"
#include "Terrain.h"
#include "World.h"

//Variables
const int HEIGHT = 720;
const int WIDTH = 1280;
float lastMouseX;
float lastMouseY;
bool mousePosInitialized = false;
float camYaw;
float camPitch;
float movementSpeed = 0.5f;
bool keyValues[1024];
double secondsSinceStart;

glm::quat camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));

//Program IDs
GLuint simpleProgram;
GLuint skyboxProgram;
GLuint terrainProgram;
GLuint modelProgram;
GLuint starProgram;
GLuint planetProgram;

//forward declaration
int InitWindow(GLFWwindow*& window);
void ProcessInput(GLFWwindow*& window);
void CreateShaders();
GLuint CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
char* ReadFile(const char* filename);
void CheckShaderCompileStatus(const GLuint& id);
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

World world;

int main()
{
	GLFWwindow* window;
	const int result = InitWindow(window);
	if (result != 0) return result;

	world.viewMatrix = glm::lookAt(world.cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	world.projectionMatrix = glm::perspective(glm::radians(60.0f), WIDTH / (float)HEIGHT, 0.1f, 5000.0f);
	world.cameraPosition = glm::vec3(0.0f, 0.0f, -10.0f);
	world.lightDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
	world.sunColor = glm::vec3(1.0f, 0.75f, 0.8f);

	CreateShaders();

	glViewport(0, 0, WIDTH, HEIGHT);

	const std::vector<std::string> cubemapPaths =
	{
		"textures/space-cubemap/right.png",
		"textures/space-cubemap/left.png",
		"textures/space-cubemap/top.png",
		"textures/space-cubemap/bottom.png",
		"textures/space-cubemap/front.png",
		"textures/space-cubemap/back.png",
	};

	Material weirdCubeMaterial(
		"textures/weirdCube_diffuse.png",
		"textures/weirdCube_normal.png",
		"textures/weirdCube_roughness.png",
		"textures/weirdCube_roughness.png",
		"textures/weirdCube_ao.png"
	);

	Skybox skybox(skyboxProgram);

	ModelActor weirdCube("weirdCube", "models/weird cube/weirdCube.fbx", modelProgram, &weirdCubeMaterial);
	weirdCube.position = glm::vec3(0, 10, 0);

	Terrain terrain("terrain", terrainProgram, "textures/heightmap.png", "textures/heightnormal.png",
		std::vector<const char*>
	{
		"textures/dirt.jpg",
			"textures/grass.png",
			"textures/sand.jpg",
			"textures/rock.jpg",
			"textures/snow.jpg"
	});

	while (!glfwWindowShouldClose(window))
	{
		//input
		ProcessInput(window);

		//poll events
		glfwPollEvents();

		//render background color
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.RenderSkybox(world);
		weirdCube.Render(world);
		terrain.RenderTerrain(world);

		//swap buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

int InitWindow(GLFWwindow*& window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics Programming", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "er is iets fout gegaan! Oh nee!" << std::endl;
		glfwTerminate();
		return -1;
	}

	//register callbacks
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetKeyCallback(window, KeyCallback);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error loading GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	return 0;
}

void CreateShaders()
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
}

char* ReadFile(const char* filename)
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

void CheckShaderCompileStatus(const GLuint& id)
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

GLuint CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
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

void CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	const float currentX = (float)xPos;
	const float currentY = (float)yPos;

	if (!mousePosInitialized)
	{
		lastMouseX = currentX;
		lastMouseY = currentY;
		mousePosInitialized = true;
	}

	float xDiff = currentX - lastMouseX;
	float yDiff = currentY - lastMouseY;
	lastMouseX = currentX;
	lastMouseY = currentY;

	camYaw -= xDiff;
	if (camYaw > 180.0f) camYaw -= 360.0f;
	if (camYaw < -180.0f) camYaw += 360.0f;

	camPitch = glm::clamp(camPitch + yDiff, -90.0f, 90.0f);

	camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));

	const glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1);
	const glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
	world.viewMatrix = glm::lookAt(world.cameraPosition, world.cameraPosition + camForward, camUp);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keyValues[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keyValues[key] = false;
	}
}

void ProcessInput(GLFWwindow*& window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	bool recalculateViewMatrix = false;
	glm::vec3 translation;

	if (keyValues[GLFW_KEY_W])
	{
		translation = camQuat * glm::vec3(0, 0, 1);
		recalculateViewMatrix = true;
	}
	if (keyValues[GLFW_KEY_A])
	{
		translation = camQuat * glm::vec3(1, 0, 0);
		recalculateViewMatrix = true;
	}
	if (keyValues[GLFW_KEY_S])
	{
		translation = camQuat * glm::vec3(0, 0, -1);
		recalculateViewMatrix = true;
	}
	if (keyValues[GLFW_KEY_D])
	{
		translation = camQuat * glm::vec3(-1, 0, 0);
		recalculateViewMatrix = true;
	}

	if (recalculateViewMatrix)
	{
		world.cameraPosition += translation * movementSpeed;
		const glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1);
		const glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
		world.viewMatrix = glm::lookAt(world.cameraPosition, world.cameraPosition + camForward, camUp);
	}
}