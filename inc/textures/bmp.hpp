#pragma once

#include <cstdint>
#include <string>

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
	~BMP() { if (data) delete[] data; data = nullptr; }

	uint32_t        fileSize{ 0 }; //Bytes
	BMPHeader       fileHeader;
	BMPInfoHeader   infoHeader;
	BMPColourHeader colourHeader;
	uint8_t*        data{ nullptr };

	BMP& operator=(const BMP&) = delete;
	BMP(BMP&& other) noexcept { *this = std::move(other); }
	BMP& operator=(BMP&& other) noexcept {
		if (this != &other) {
			if (data) { delete[] data; }
			fileSize = other.fileSize;
			fileHeader = other.fileHeader;
			infoHeader = other.infoHeader;
			colourHeader = other.colourHeader;
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}
};

class BMPTexture {
public:
	bool createBMPTexture(std::string name, std::string path, bool generateMIPMap);
	bool createCubeBMPTexture(std::string cubeMapName,
		std::string posXfileName, std::string negXfileName,
		std::string posYfileName, std::string negYfileName,
		std::string posZfileName, std::string negZfileName,
		bool isSeamless, bool generateMIPMap);
		
	int getTextureNum() const { return textureNum; }
	std::string getTextureName() const { return textureName; }
	std::string getTexturePath() const { return texturePath; }
private:
	int textureNum{ 0 };
	std::string textureName;
	std::string texturePath;
	BMP bmp;

	bool isCubeMap{ false }, is2DTexture{ false };
};