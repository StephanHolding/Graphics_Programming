#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/gtx/quaternion.hpp>

#include "Framebuffer.h"
#include "Material.h"
#include "ImageLoader.h"
#include "ModelActor.h"
#include "Skybox.h"
#include "Terrain.h"
#include "World.h"
#include "Shaders.h"

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

//forward declaration
int InitWindow(GLFWwindow*& window);
void ProcessInput(GLFWwindow*& window);
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

World* world;
Shaders* shaders;

int main()
{
	GLFWwindow* window;
	const int result = InitWindow(window);
	if (result != 0) return result;

	glViewport(0, 0, WIDTH, HEIGHT);

	World worldInstance;
	world = &worldInstance;

	Shaders shadersInstance;
	shaders = &shadersInstance;

	world->viewMatrix = glm::lookAt(world->cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	world->projectionMatrix = glm::perspective(glm::radians(60.0f), WIDTH / (float)HEIGHT, 0.1f, 5000.0f);
	world->cameraPosition = glm::vec3(0.0f, 0.0f, -10.0f);
	world->lightDirection = glm::normalize(glm::vec3(1.0f, -0.5f, 0.0f));
	world->sunColor = glm::vec3(0.8f, 0.8f, 1.0f);

	const std::vector<std::string> cubemapPaths =
	{
		"textures/space-cubemap/right.png",
		"textures/space-cubemap/left.png",
		"textures/space-cubemap/top.png",
		"textures/space-cubemap/bottom.png",
		"textures/space-cubemap/front.png",
		"textures/space-cubemap/back.png",
	};

	Material weirdCubeMaterial
	(
		"textures/weirdCube_diffuse.png",
		"textures/weirdCube_normal.png",
		"textures/weirdCube_roughness.png",
		"textures/weirdCube_roughness.png",
		"textures/weirdCube_ao.png"
	);

	Skybox skybox(shaders->skyboxProgram);

//	ModelActor weirdCube("weirdCube", "models/weird cube/weirdCube.fbx", shaders->modelProgram, &weirdCubeMaterial);
//	weirdCube.position = glm::vec3(0, 10, 0);

	Terrain terrain("terrain", shaders->terrainProgram, "textures/heightmap.png", "textures/heightnormal.png",
		std::vector<const char*>
	{
		"textures/dirt.jpg",
			"textures/grass.png",
			"textures/sand.jpg",
			"textures/rock.jpg",
			"textures/snow.jpg"
	});

	Framebuffer normalBuffer(WIDTH, HEIGHT, shaders->blitProgram);
	normalBuffer.CreateBuffers();

	Framebuffer hdrBuffer(WIDTH, HEIGHT, shaders->hdrShader);
	hdrBuffer.CreateBuffers(GL_RGBA16F);

	while (!glfwWindowShouldClose(window))
	{
		//input
		ProcessInput(window);

		//poll events
		glfwPollEvents();

		normalBuffer.EnableBuffer();

			//render background color
			glClearColor(0.0, 0.0, 0.0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Render Scene
			skybox.RenderSkybox(*world);
			//weirdCube.Render(*world);
			terrain.RenderTerrain(*world);

		normalBuffer.DisableBuffer();

		normalBuffer.RenderToScreen();

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
	world->viewMatrix = glm::lookAt(world->cameraPosition, world->cameraPosition + camForward, camUp);
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
		world->cameraPosition += translation * movementSpeed;
		const glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1);
		const glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
		world->viewMatrix = glm::lookAt(world->cameraPosition, world->cameraPosition + camForward, camUp);
	}
}