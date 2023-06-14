#include "ModelActor.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

ModelActor::ModelActor(const std::string& name, const char* modelPath, const GLuint programID, Material* material): Actor(name)
{
	SetModel(modelPath, programID, material);
}

ModelActor::~ModelActor()
{
	delete model;
}

void ModelActor::Render(World& world) const
{
	if (model == nullptr)
		return;

	if (material->useAlpha)
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

	glUseProgram(programID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = modelMatrix * glm::toMat4(glm::quat(rotation));
	modelMatrix = glm::scale(modelMatrix, scale);

	glUniformMatrix4fv(glGetUniformLocation(programID, "world"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(world.viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(world.projectionMatrix));

	glUniform3fv(glGetUniformLocation(programID, "lightDirection"), 1, glm::value_ptr(world.lightDirection));
	glUniform3fv(glGetUniformLocation(programID, "cameraPosition"), 1, glm::value_ptr(world.cameraPosition));
	glUniform3fv(glGetUniformLocation(programID, "sunColor"), 1, glm::value_ptr(world.sunColor));
	glUniform3fv(glGetUniformLocation(programID, "color_diffuse"), 1, glm::value_ptr(material->diffuseColor));
	glUniform1f(glGetUniformLocation(programID, "normal_strength"), material->normalStrength);

	material->BindTextures();

	model->Draw(programID);

	if (material->useAlpha)
	{
		glDisable(GL_BLEND);
	}
}

void ModelActor::SetModel(const char* modelPath, const GLuint programID, Material* material)
{
	model = new Model(modelPath);
	this->programID = programID;
	this->material = material;
}