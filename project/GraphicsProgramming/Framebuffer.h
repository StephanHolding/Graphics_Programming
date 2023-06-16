#pragma once
#include <glad/glad.h>

class Framebuffer
{
public:
	Framebuffer(const int width, const int height, const GLuint programID);

	void CreateBuffers(const GLuint colorBufferSize = GL_RGBA);
	void SetCustomShader(const GLuint customShader);
	void EnableBuffer();
	void DisableBuffer();
	void RenderToScreen();
	void RenderFromOtherBuffer(Framebuffer& otherBuffer);
	GLuint& GetColorBufferID();

private:

	GLuint frameBufferID;
	GLuint rectangleVAO;
	GLuint rectangleVBO;
	GLuint colorBufferID;
	GLuint programID;

	const int width;
	const int height;
	const GLuint blitProgramID;
	const float rectangleVertices[20];


	void CheckFramebufferStatus();
	void CreateRectangle();

};

