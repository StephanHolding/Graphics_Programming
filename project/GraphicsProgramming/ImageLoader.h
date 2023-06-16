#pragma once
#include <glad/glad.h>
#include <vector>
#include <iostream>

class ImageLoader
{
public:
	static GLuint LoadTexture(const char* path, int comp = 0, bool flipVertically = false);
	static GLuint LoadCubemap(std::vector<std::string> paths, int comp = 0);
	static GLuint CreateEmptyTexture();
};

