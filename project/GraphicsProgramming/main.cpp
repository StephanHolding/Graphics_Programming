#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <time.h>

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

glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -10.0f);
glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 0.8f);
glm::quat camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));

//skybox geometry data:
GLuint skyboxVAO;
GLuint skyboxEBO;
int skyboxMemorySize;
int skyboxNumofIndeces;

//small box data:
GLuint boxVAO;
GLuint boxEBO;
int boxMemorySize;
int boxNumofIndeces;

//terrain data
GLuint terrainVAO;
GLuint terrainIndexCount;
GLuint heightmapID;
GLuint heightNormalID;
unsigned char* heightMapTexture;

//global matrices
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

//Program IDs
GLuint simpleProgram;
GLuint skyboxProgram;
GLuint terrainProgram;
GLuint modelProgram;
GLuint starProgram;
GLuint planetProgram;

//textures
GLuint boxTexture;
GLuint boxNormalTexture;
GLuint dirtT, sandT, grassT, rockT, snowT;
GLuint starsCubemap;
GLuint dayT, nightT, cloudsT, moonT;
GLuint emptyTexture;

//models
Model* box;
Model* weirdCube;
Model* backpack;
Model* sphere;

//forward declaration
int InitWindow(GLFWwindow*& window);
void ProcessInput(GLFWwindow*& window);
void CreateGeometry(GLuint& vao, GLuint& ebo, int& size, int& numOfindices);
void CreateShaders();
GLuint CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
char* ReadFile(const char* filename);
void CheckShaderCompileStatus(const GLuint& id);
GLuint LoadTexture(const char* path, int comp = 0, bool flipVertically = false);
GLuint LoadCubemap(std::vector<string> paths, int comp = 0);
void RenderSkybox(const GLuint& program);
void RenderSkybox(const GLuint& program, const GLuint& cubemap);
void RenderBox();
void RenderTerrain();
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int GenerateTerrain(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);
void RenderModel(Model* model, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale, const GLuint program, const std::vector<GLuint>* textures = nullptr, bool alpha = false);
GLuint CreateEmptyTexture();
//void RenderPlanet(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

int main()
{
	GLFWwindow* window;
	const int result = InitWindow(window);
	if (result != 0) return result;

	time_t start = time(nullptr);

	emptyTexture = CreateEmptyTexture();
	CreateShaders();
	CreateGeometry(skyboxVAO, skyboxEBO, skyboxMemorySize, skyboxNumofIndeces);
	CreateGeometry(boxVAO, boxEBO, boxMemorySize, boxNumofIndeces);


	glViewport(0, 0, WIDTH, HEIGHT);

	viewMatrix = glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(glm::radians(60.0f), WIDTH / (float)HEIGHT, 0.1f, 5000.0f);

	std::vector<string> cubemapPaths =
	{
		"textures/space-cubemap/right.png",
		"textures/space-cubemap/left.png",
		"textures/space-cubemap/top.png",
		"textures/space-cubemap/bottom.png",
		"textures/space-cubemap/front.png",
		"textures/space-cubemap/back.png",
	};

	starsCubemap = LoadCubemap(cubemapPaths);

	terrainVAO = GenerateTerrain("textures/heightmap.png", heightMapTexture, GL_RGBA, 4, 250.0f, 5.0f, terrainIndexCount, heightmapID);
	heightNormalID = LoadTexture("textures/heightnormal.png", 0, true);

	boxTexture = LoadTexture("textures/metalBox.jpg");
	boxNormalTexture = LoadTexture("textures/metalBoxNormal.png");

	dirtT = LoadTexture("textures/dirt.jpg");
	sandT = LoadTexture("textures/sand.jpg");
	grassT = LoadTexture("textures/grass.png", 4);
	rockT = LoadTexture("textures/rock.jpg");
	snowT = LoadTexture("textures/snow.jpg");

	dayT = LoadTexture("textures/day.jpg");
	nightT = LoadTexture("textures/night.jpg");
	cloudsT = LoadTexture("textures/clouds.jpg");
	moonT = LoadTexture("textures/2k_moon.jpg");

	std::vector<GLuint> earthTextures =
	{
		dayT,
		nightT,
		cloudsT
	};

	std::vector<GLuint> metalBoxTextures =
	{
		LoadTexture("textures/metalBox.jpg"),
		LoadTexture("textures/metalBoxNormal.png"),
		emptyTexture,
		emptyTexture,
		emptyTexture
	};

	std::vector<GLuint> backpackTextures =
	{
		LoadTexture("models/backpack/diffuse.jpg"),
		LoadTexture("models/backpack/normal.png"),
		LoadTexture("models/backpack/specular.jpg"),
		LoadTexture("models/backpack/ao.jpg"),
		LoadTexture("models/backpack/roughness.jpg"),
	};

	const std::vector<GLuint> weirdCubeTextures =
	{
		LoadTexture("textures/weirdCube_diffuse.png"),
		LoadTexture("textures/weirdCube_normal.png"),
		LoadTexture("textures/weirdCube_roughness.png"),
		LoadTexture("textures/weirdCube_roughness.png"),
		LoadTexture("textures/weirdCube_ao.png"),
	};

	backpack = new Model("models/backpack/backpack.obj");
	sphere = new Model("models/planets/uv_sphere.obj");
	box = new Model("models/box/box.fbx");
	weirdCube = new Model("models/weird cube/weirdCube.fbx");

	while (!glfwWindowShouldClose(window))
	{
		//input
		ProcessInput(window);

		//poll events
		glfwPollEvents();

		secondsSinceStart = difftime(time(nullptr), start);

		//render background color
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderSkybox(skyboxProgram);
		//RenderBox();
		//RenderSkybox(starProgram, starsCubemap);
		RenderModel(weirdCube, glm::vec3(0, 0, 0), glm::vec3(0, 45, 0), glm::vec3(10, 10, 10), modelProgram, &weirdCubeTextures);
		//RenderModel(sphere, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100), planetProgram, earthTextures);
		RenderTerrain();
		//RenderModel(backpack, glm::vec3(100, 100, 100), glm::vec3(0, -90.0f, 0), glm::vec3(100, 100, 100), modelProgram);

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

void CreateGeometry(GLuint& vao, GLuint& ebo, int& size, int& numOfindices)
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals          //tangents			//bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,		0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,		0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,		0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,		 0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,		0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,		0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,		0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,		0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,		0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,		0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,		 0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,		0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,		0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,		0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,		0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,		0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,		 0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,		0.f, 0.f, 1.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f
	};

	const unsigned int indices[] = {  // note that we start from 0!
		// DOWN
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
		// BACK
		14, 6, 7,   // first triangle
		14, 7, 15,    // second triangle
		// RIGHT
		20, 4, 5,   // first triangle
		20, 5, 21,    // second triangle
		// LEFT
		16, 8, 9,   // first triangle
		16, 9, 17,    // second triangle
		// FRONT
		18, 10, 11,   // first triangle
		18, 11, 19,    // second triangle
		// UP
		22, 12, 13,   // first triangle
		22, 13, 23,    // second triangle
	};

	constexpr int positionSize = 3;
	constexpr int colorSize = 3;
	constexpr int texCoordsSize = 2;
	constexpr int normalsSize = 3;
	constexpr int tangentsSize = 3;
	constexpr int bitangentsSize = 3;
	constexpr int vertexSize = positionSize + colorSize + texCoordsSize + normalsSize + tangentsSize + bitangentsSize;
	constexpr int stride = vertexSize * sizeof(float);


	//GLuint VAO; //Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo; //Vertex Buffer Object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, positionSize, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, colorSize, GL_FLOAT, GL_FALSE, stride, (void*)(positionSize * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, texCoordsSize, GL_FLOAT, GL_FALSE, stride, (void*)((positionSize + colorSize) * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, normalsSize, GL_FLOAT, GL_TRUE, stride, (void*)((positionSize + colorSize + texCoordsSize) * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, tangentsSize, GL_FLOAT, GL_FALSE, stride, (void*)((positionSize + colorSize + texCoordsSize + normalsSize) * sizeof(float)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, bitangentsSize, GL_FLOAT, GL_FALSE, stride, (void*)((positionSize + colorSize + texCoordsSize + normalsSize + tangentsSize) * sizeof(float)));
	glEnableVertexAttribArray(5);

	size = sizeof(vertices) / stride;
	numOfindices = sizeof(indices) / sizeof(int);
}

void RenderSkybox(const GLuint& program)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(program);

	glm::mat4 skyboxWorldMatrix = glm::mat4(1.0f);
	skyboxWorldMatrix = glm::translate(skyboxWorldMatrix, cameraPosition);
	skyboxWorldMatrix = glm::scale(skyboxWorldMatrix, glm::vec3(10, 10, 10));

	glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(skyboxWorldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3fv(glGetUniformLocation(program, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	glUniform3fv(glGetUniformLocation(program, "sunColor"), 1, glm::value_ptr(sunColor));

	glBindVertexArray(skyboxVAO);
	glDrawElements(GL_TRIANGLES, skyboxNumofIndeces, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
}

void RenderSkybox(const GLuint& program, const GLuint& cubemap)
{
	RenderSkybox(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}

void RenderBox()
{
	//set values of matrices
	glm::mat4 boxWorldMatrix = glm::mat4(1.0f);
	boxWorldMatrix = glm::rotate(boxWorldMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	boxWorldMatrix = glm::scale(boxWorldMatrix, glm::vec3(10, 10, 10));
	boxWorldMatrix = glm::translate(boxWorldMatrix, glm::vec3(0, 0, 0));

	glUseProgram(simpleProgram);

	glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "world"), 1, GL_FALSE, glm::value_ptr(boxWorldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3fv(glGetUniformLocation(simpleProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(simpleProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, boxNormalTexture);

	//draw elements
	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxNumofIndeces, GL_UNSIGNED_INT, 0);
}

void RenderTerrain()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(terrainProgram);

	glm::mat4 terrainWorldMatrix = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(terrainWorldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3fv(glGetUniformLocation(terrainProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(terrainProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	glUniform3fv(glGetUniformLocation(terrainProgram, "sunColor"), 1, glm::value_ptr(sunColor));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmapID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightNormalID);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dirtT);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sandT);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grassT);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rockT);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, snowT);

	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
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
	glUniform1i(glGetUniformLocation(modelProgram, "texture_diffuse1"), 0);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_normal1"), 1);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_specular1"), 2);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_roughness1"), 3);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_ao1"), 4);

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

GLuint LoadTexture(const char* path, const int comp, const bool flipVertically)
{
	stbi_set_flip_vertically_on_load(flipVertically);

	GLuint toReturn;
	glGenTextures(1, &toReturn);
	glBindTexture(GL_TEXTURE_2D, toReturn);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, comp);
	if (data)
	{
		if (comp != 0)
		{
			numChannels = comp;
		}

		if (numChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (numChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading texture: " << path << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_set_flip_vertically_on_load(false);

	return toReturn;
}

GLuint CreateEmptyTexture()
{
	GLuint toReturn;
	const GLubyte texData[] = { 255, 255, 255, 255 };

	glGenTextures(1, &toReturn);
	glBindTexture(GL_TEXTURE_2D, toReturn);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	return toReturn;
}

GLuint LoadCubemap(std::vector<string> paths, int comp)
{
	GLuint toReturn;
	glGenTextures(1, &toReturn);
	glBindTexture(GL_TEXTURE_CUBE_MAP, toReturn);

	int width, height, numChannels;

	for (int i = 0; i < paths.size(); i++)
	{
		const char* path = paths.at(i).c_str();

		unsigned char* data = stbi_load(path, &width, &height, &numChannels, comp);
		if (data)
		{
			if (comp != 0)
			{
				numChannels = comp;
			}

			if (numChannels == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if (numChannels == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			//glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Error loading texture: " << path << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return toReturn;
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
	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + camForward, camUp);
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
		cameraPosition += translation * movementSpeed;
		const glm::vec3 camForward = camQuat * glm::vec3(0, 0, 1);
		const glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + camForward, camUp);
	}
}

unsigned int GenerateTerrain(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	data = nullptr;

	if (heightmap != nullptr) {
		data = stbi_load(heightmap, &width, &height, &channels, comp);
		if (data) {
			glGenTextures(1, &heightmapID);
			glBindTexture(GL_TEXTURE_2D, heightmapID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0;
	for (int i = 0; i < (width * height); i++)
	{
		// TODO: calculate x/z values

		int x = i % width;
		int z = i / width;

		float texHeight = data[i * comp];

		// TODO: set position
		vertices[index++] = x * xzScale;
		vertices[index++] = (texHeight / 255.0f) * hScale;
		vertices[index++] = z * xzScale;

		// TODO: set normal
		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		// TODO: set uv
		vertices[index++] = x / (float)width;
		vertices[index++] = z / (float)height;
	}

	// OPTIONAL TODO: Calculate normal
	// TODO: Set normal

	index = 0;
	for (int i = 0; i < (width - 1) * (height - 1); i++)
	{
		int x = i % (width - 1);
		int z = i / (width - 1);

		int vertex = z * width + x;

		indices[index++] = vertex;
		indices[index++] = vertex + width;
		indices[index++] = vertex + width + 1;

		indices[index++] = vertex;
		indices[index++] = vertex + width + 1;
		indices[index++] = vertex + 1;
	}

	unsigned int vertSize = (width * height) * stride * sizeof(float);
	indexCount = ((width - 1) * (height - 1) * 6);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// vertex information!
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete[] vertices;
	delete[] indices;

	stbi_set_flip_vertically_on_load(false);

	return VAO;
}

void RenderModel(Model* model, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale, const GLuint program, const std::vector<GLuint>* textures, bool alpha)
{
	if (alpha)
	{
		glEnable(GL_BLEND);

		//alpha blend
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//additive blending
		//glBlendFunc(GL_ONE, GL_ONE);

		// soft additive blending
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

		// MULTIPLY BLENDING
		//glBlendFunc(GL_DST_COLOR, GL_ZERO);
	}

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(program);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = modelMatrix * glm::toMat4(glm::quat(rotation));
	modelMatrix = glm::scale(modelMatrix, scale);

	glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	float offset = secondsSinceStart * 0.01f;
	glUniform1f(glGetUniformLocation(program, "offset"), offset);

	glUniform3fv(glGetUniformLocation(program, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	glUniform3fv(glGetUniformLocation(program, "sunColor"), 1, glm::value_ptr(sunColor));

	if (textures != nullptr)
	{
		for (int i = 0; i < textures->size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures->at(i));
		}
	}

	model->Draw(program);

	if (alpha)
	{
		glDisable(GL_BLEND);
	}
}

/*
void RenderPlanet(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(planetProgram);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = modelMatrix * glm::toMat4(glm::quat(rotation));
	modelMatrix = glm::scale(modelMatrix, scale);

	glUniformMatrix4fv(glGetUniformLocation(planetProgram, "world"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(planetProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(planetProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3fv(glGetUniformLocation(planetProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(planetProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dayT);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, nightT);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cloudsT);

	model->Draw(planetProgram);

	glDisable(GL_BLEND);
}
*/

