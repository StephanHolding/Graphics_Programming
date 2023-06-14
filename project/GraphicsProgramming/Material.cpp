#include "Material.h"
#include "Loader.h"


Material::Material(const char* diffusePath, const char* normalPath, const char* specularPath,
                   const char* roughnessPath, const char* aoPath)
{
	this->emptyTex = Loader::CreateEmptyTexture();
	this->useAlpha = false;
	this->diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->normalStrength = 1.0f;

	textureIDs =
	{
		EmptyPathCheck(diffusePath),
		EmptyPathCheck(normalPath),
		EmptyPathCheck(specularPath),
		EmptyPathCheck(roughnessPath),
		EmptyPathCheck(aoPath)
	};
}

void Material::BindTextures() const
{
	for (int i = 0; i < textureIDs.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureIDs.at(i));
	}
}

GLuint Material::EmptyPathCheck(const char* path) const
{
	if (path != "")
		return Loader::LoadTexture(path);
	else
		return emptyTex;
}
