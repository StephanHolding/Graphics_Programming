#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

GameObject::GameObject(std::string objectName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	this->objectName = objectName;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}

void GameObject::PrepareRendering(Model* model, GLuint programID)
{
	toRender = model;
	this->programID = programID;
}


void GameObject::Render()
{
	if (toRender != nullptr && programID != 0)
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
}
