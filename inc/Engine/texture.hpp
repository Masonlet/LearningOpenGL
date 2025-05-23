#pragma once

class Texture {
	unsigned int width, height, nrChannels;
	unsigned char bitDepth, colourType, compression, filter, interlace;
	unsigned char* parsePNG(unsigned char* buffer, size_t fileSize);

public:
	bool load(const char* path);
};