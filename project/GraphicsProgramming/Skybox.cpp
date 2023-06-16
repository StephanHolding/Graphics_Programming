#include "Skybox.h"
#include <glm/gtc/type_ptr.hpp>

#include "ImageLoader.h"

Skybox::Skybox(GLuint programID)
{
	this->programID = programID;
	CreateSkyboxGeometry();
}

Skybox::Skybox(GLuint programID, const std::vector<std::string>& cubemapPaths)
{
	this->programID = programID;
	CreateSkyboxGeometry();
	cubemap = ImageLoader::LoadCubemap(cubemapPaths);
}

void Skybox::CreateSkyboxGeometry()
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
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	GLuint vbo; //Vertex Buffer Object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &skyboxEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
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

	skyboxMemorySize = sizeof(vertices) / stride;
	skyboxNumofIndeces = sizeof(indices) / sizeof(int);
}

void Skybox::RenderSkybox(World& world)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(programID);

	glm::mat4 skyboxWorldMatrix = glm::mat4(1.0f);
	skyboxWorldMatrix = glm::translate(skyboxWorldMatrix, world.cameraPosition);
	skyboxWorldMatrix = glm::scale(skyboxWorldMatrix, glm::vec3(10, 10, 10));

	glUniformMatrix4fv(glGetUniformLocation(programID, "world"), 1, GL_FALSE, glm::value_ptr(skyboxWorldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(world.viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(world.projectionMatrix));

	glUniform3fv(glGetUniformLocation(programID, "lightDirection"), 1, glm::value_ptr(world.lightDirection));
	glUniform3fv(glGetUniformLocation(programID, "cameraPosition"), 1, glm::value_ptr(world.cameraPosition));
	glUniform3fv(glGetUniformLocation(programID, "sunColor"), 1, glm::value_ptr(world.sunColor));

	glBindVertexArray(skyboxVAO);
	glDrawElements(GL_TRIANGLES, skyboxNumofIndeces, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	if (cubemap != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	}
}
