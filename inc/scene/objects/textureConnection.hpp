#pragma once

#include <string>

struct TextureConnection {
	std::string name;
	std::string modelName, textureName;
	unsigned int slot{ 0 };
	float mix{ 1.0f };
};