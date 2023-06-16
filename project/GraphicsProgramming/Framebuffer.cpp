#include "Framebuffer.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Framebuffer::Framebuffer(const int width, const int height, const GLuint blitProgram)
	: width(width), height(height), blitProgramID(blitProgram), rectangleVertices
	  {
		  // vert pos				//TexCoords
		  -1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
		  -1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		  1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		  1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	  }
{
	CreateRectangle();
}

void Framebuffer::CreateBuffers(const GLuint colorBufferSize)
{
	//create frame buffer
	glGenFramebuffers(1, &frameBufferID);

	//create color buffer
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, colorBufferSize , width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

	//create depth buffer
	GLuint depthBufferID;
	glGenRenderbuffers(1, &depthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	//bind buffers
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

	CheckFramebufferStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::SetCustomShader(const GLuint customShader)
{
	programID = customShader;
}

void Framebuffer::EnableBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

void Framebuffer::DisableBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RenderToScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(blitProgramID);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);

	glBindVertexArray(rectangleVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RenderFromOtherBuffer(Framebuffer& otherBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	GLuint programToUse;

	if (programID != 0)
		programToUse = programID;
	else
		programToUse = blitProgramID;

	glUseProgram(programToUse);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, otherBuffer.GetColorBufferID());

	glBindVertexArray(rectangleVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint& Framebuffer::GetColorBufferID()
{
	return colorBufferID;
}

void Framebuffer::CheckFramebufferStatus()
{
	const auto frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer error: " << frameBufferStatus << std::endl;
	}
}

void Framebuffer::CreateRectangle()
{
	glGenVertexArrays(1, &rectangleVAO);
	glGenBuffers(1, &rectangleVBO);
	glBindVertexArray(rectangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}
