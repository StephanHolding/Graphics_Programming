#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Variables
const int HEIGHT = 720;
const int WIDTH = 1280;

//Program IDs
GLuint simpleProgram;

//forward declaration
int InitWindow(GLFWwindow*& window);
void CreateGeometry(GLuint& vao, GLuint& ebo, int& size, int& numOfindices);
void CreateShaders();
GLuint CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
char* ReadFile(const char* filename);
void CheckShaderCompileStatus(const GLuint& id);
GLuint LoadTexture(const char* path);

int main()
{
	GLFWwindow* window;
	const int result = InitWindow(window);
	if (result != 0) return result;

	GLuint quadVAO;
	GLuint quadEBO;
	int quadSize;
	int numOfIndices;
	CreateGeometry(quadVAO, quadEBO, quadSize, numOfIndices);

	CreateShaders();

	GLuint boxTexture = LoadTexture("textures/box.png");
	GLuint boxNormalTexture = LoadTexture("textures/boxNormal.png");
	
	glViewport(0, 0, WIDTH, HEIGHT);

	glm::vec3 lightPosition = glm::vec3(2, 5, 3);
	glm::vec3 cameraPosition = glm::vec3(0, 2.5f, -5.0f);

	//Matrices
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix = glm::rotate(worldMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	worldMatrix = glm::scale(worldMatrix, glm::vec3(1, 1, 1));
	worldMatrix = glm::translate(worldMatrix, glm::vec3(0, 0, 0));

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 projectionMatrix = glm::perspective( glm::radians(30.0f), WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		//input

		//poll events
		glfwPollEvents();

		//rendering
		//render background color
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(simpleProgram);
		glUniform1i(glGetUniformLocation(simpleProgram, "mainTex"), 0);
		glUniform1i(glGetUniformLocation(simpleProgram, "normalTex"), 1);

		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "world"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glUniform3fv(glGetUniformLocation(simpleProgram, "lightPosition"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(simpleProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, boxNormalTexture);

		//draw elements
		glBindVertexArray(quadVAO);
		glDrawElements(GL_TRIANGLES, numOfIndices, GL_UNSIGNED_INT, 0);

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
		// positions            //colors            // tex coords   // normals          //tangents      //bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

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

void CreateShaders()
{
	simpleProgram = CreateProgram("shaders/vertex.shader", "shaders/fragment.shader");
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

GLuint LoadTexture(const char* path)
{
	GLuint toReturn;
	glGenTextures(1, &toReturn);
	glBindTexture(GL_TEXTURE_2D, toReturn);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);
	if (data)
	{
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

	return toReturn;
}


