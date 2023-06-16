#include "ImageLoader.h"

#include "stb_image.h"
//#define STB_IMAGE_IMPLEMENTATION

GLuint ImageLoader::LoadTexture(const char* path, const int comp, const bool flipVertically)
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

GLuint ImageLoader::LoadCubemap(const std::vector<std::string> paths, const int comp)
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

GLuint ImageLoader::CreateEmptyTexture()
{
	GLuint toReturn;
	constexpr GLubyte texData[] = { 255, 255, 255, 255 };

	glGenTextures(1, &toReturn);
	glBindTexture(GL_TEXTURE_2D, toReturn);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	return toReturn;
}
