#pragma once

struct Texture {
	unsigned int width, height, nrChannels;

	bool createTexture(const char* path);
};