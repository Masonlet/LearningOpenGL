#pragma once

#include "math/vec2.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct BMPHeader {
	uint32_t offsetData{ 0 }; // Start pos of pixel data
};

struct BMPInfoHeader {
	uint32_t size{ 0 };							  // Bytes
	int32_t  width{ 0 }, height{ 0 }; // Pixels	    
	uint16_t bitCount{ 0 };				    // Bits per pixel   
	int32_t  xPixelsPerMeter{ 0 }; 
	int32_t  yPixelsPerMeter{ 0 };
	uint32_t colorsUsed{ 0 };			 // 0 (all colors) or number of color indexes in the colour table
	uint32_t colorsImportant{ 0 }; // 0 (all colors are important) or number of important color indexes
};

struct BMPColourHeader {
	uint32_t redChannelMask{ 0x00FF0000 }; 
	uint32_t greenChannelMask{ 0x0000FF00 };
	uint32_t blueChannelMask{ 0x000000FF };
	uint32_t alphaChannelMask{ 0xFF000000 };
	uint32_t colourSpaceType{ 0x73524742 }; // Default "sRGB"
	uint32_t unused[16]{ 0 };							  // Unused data for sRGB color space
};

struct BMP {
	BMP() = default;
	BMP(const BMP&) = delete;

	uint32_t        fileSize{ 0 };
	BMPHeader       fileHeader;
	BMPInfoHeader   infoHeader;
	BMPColourHeader colourHeader;

	std::vector<uint8_t> pixels;

	BMP& operator=(const BMP&) = delete;
	BMP(BMP&& other) noexcept { *this = std::move(other); }
	BMP& operator=(BMP&& other) noexcept {
		if (this != &other) {
			fileSize = other.fileSize;
			fileHeader = other.fileHeader;
			infoHeader = other.infoHeader;
			colourHeader = other.colourHeader;
		}
		return *this;
	}
};

struct BMPTexture {
	std::string name;
	unsigned int index{ 0 }, slot{ 0 };
	float mix{ 0.0f };
	Vec2 tiling{ 0.0f };
	bool isCube{ false };
	int width{ 0 }, height{ 0 };

	bool createBMPTexture(std::string path, bool generateMIPMap);
	bool createBMPCubeTexture(std::string posXfileName, std::string negXfileName, 
		std::string posYfileName, std::string negYfileName, 
		std::string posZfileName, std::string negZfileName,	
		bool isSeamless, bool generateMIPMap);
};
