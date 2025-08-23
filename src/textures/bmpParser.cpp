#include "textures/bmpParser.hpp"
#include "textures/bmp.hpp"
#include "utils/fileParser.hpp"
#include "utils/log.hpp"
#include <cstdio>

static bool readHeader(const unsigned char* buffer, int& height, int& width) {
	if (!buffer) return error("BMPParser", "readHeader", "Null buffer");
	if (buffer[0] != 'B' || buffer[1] != 'M') return error("BMPParser", "readHeader", "Bad signature (not BM)");

	unsigned int dibSize =
		(unsigned int)buffer[14] |
		((unsigned int)buffer[15] << 8) |
		((unsigned int)buffer[16] << 16) |
		((unsigned int)buffer[17] << 24);
	if (dibSize < 40U) return error("BMPParser", "readHeader", "Unsupported DIB header size: " + std::to_string(dibSize));

	int32_t w = static_cast<int32_t>(
		static_cast<unsigned int>(buffer[18]) |
		(static_cast<unsigned int>(buffer[19]) << 8) |
		(static_cast<unsigned int>(buffer[20]) << 16) |
		(static_cast<unsigned int>(buffer[21]) << 24)
	);
	if (w <= 0) return error("BMPParser", "readHeader", "Invalid width: " + std::to_string(w));

	int32_t h = static_cast<int32_t>(
		static_cast<unsigned int>(buffer[22]) |
		(static_cast<unsigned int>(buffer[23]) << 8) |
		(static_cast<unsigned int>(buffer[24]) << 16) |
		(static_cast<unsigned int>(buffer[25]) << 24)
	);
	if (h == 0) return error("BMPParser", "readHeader",  "Invalid height: " + std::to_string(h));

	unsigned short planes = static_cast<unsigned short>(
		static_cast<unsigned int>(buffer[26]) |
		(static_cast<unsigned int>(buffer[27]) << 8)
	);
	if (planes != 1) return error("BMPParser", "readHeader", "Planes != 1: " + std::to_string(planes));

	unsigned short bpp = (unsigned short)(
		static_cast<unsigned int>(buffer[28]) |
		(static_cast<unsigned int>(buffer[29]) << 8)
	);
	if (bpp != 24) return error("BMPParser", "readHeader", "Only 24bpp supported: " + std::to_string(bpp));

	unsigned int compression =
		static_cast<unsigned int>(buffer[30]) |
		(static_cast<unsigned int>(buffer[31]) << 8) |
		(static_cast<unsigned int>(buffer[32]) << 16) |
		(static_cast<unsigned int>(buffer[33]) << 24
	);
	if (compression != 0) return error("BMPParser", "readHeader", "Compressed BMP not supported: " + std::to_string(compression));

	width = static_cast<int>(w);
	height = (h > 0) ? static_cast<int>(h) 
									 : static_cast<int>(-h); 
	return true;
}

bool parseBMP(const char* filename, BMP& bmpOut) {
	const unsigned char* buffer{ nullptr };
	size_t fileSize{ 0 };

	if (!loadBinaryFile(buffer, fileSize, filename) || !buffer)
		return error("BMPParser", "parseBMP", std::string("Failed to load file: ") + filename);

	if (fileSize < 54) {
		delete[] buffer;
		return error("BMPParser", "parseBMP", "File too small to be a valid BMP: " + std::to_string(fileSize) + " bytes");
	}   

	// Check 54-byte header
	int height{ 0 }, width{ 0 };
	if (!readHeader(buffer, height, width)) {
		delete[] buffer;
		return false;
	}

	unsigned int dataOffset =
		static_cast<unsigned int>(buffer[10]) |
		(static_cast<unsigned int>(buffer[11]) << 8) |
		(static_cast<unsigned int>(buffer[12]) << 16) |
		(static_cast<unsigned int>(buffer[13]) << 24);
	if (dataOffset >= fileSize) {
		delete[] buffer;
		return error("BMPParser", "parseBMP", "Bad data offset: " + std::to_string(dataOffset) + " bytes");
	}

	unsigned int dibSize =
		static_cast<unsigned int>(buffer[14]) |
		(static_cast<unsigned int>(buffer[15]) << 8) |
		(static_cast<unsigned int>(buffer[16]) << 16) |
		(static_cast<unsigned int>(buffer[17]) << 24);

	int32_t signedHeight =(int32_t)(
		static_cast<unsigned int>(buffer[22]) |
		(static_cast<unsigned int>(buffer[23]) << 8) |
		(static_cast<unsigned int>(buffer[24]) << 16) |
		(static_cast<unsigned int>(buffer[25]) << 24)
	);
	int bottomUp = (signedHeight > 0) ? 1 : 0;
		
	int32_t xppm = 0;
	int32_t yppm = 0;
	if (dibSize >= 40U) {
		xppm = (int32_t)(
			static_cast<unsigned int>(buffer[38]) |
			(static_cast<unsigned int>(buffer[39]) << 8) |
			(static_cast<unsigned int>(buffer[40]) << 16) |
			(static_cast<unsigned int>(buffer[41]) << 24)
		);
		yppm = (int32_t)(
			static_cast<unsigned int>(buffer[42]) |
			(static_cast<unsigned int>(buffer[43]) << 8) |
			(static_cast<unsigned int>(buffer[44]) << 16) |
			(static_cast<unsigned int>(buffer[45]) << 24)
		);
	}

	unsigned int colorsUsed =
		static_cast<unsigned int>(buffer[46]) |
		(static_cast<unsigned int>(buffer[47]) << 8) |
		(static_cast<unsigned int>(buffer[48]) << 16) |
		(static_cast<unsigned int>(buffer[49]) << 24);

	unsigned int colorsImportant =
		static_cast<unsigned int>(buffer[50]) |
		(static_cast<unsigned int>(buffer[51]) << 8) |
		(static_cast<unsigned int>(buffer[52]) << 16) |
		(static_cast<unsigned int>(buffer[53]) << 24);

	if (dataOffset > fileSize) {
		delete[] buffer;
		return error("BMPParser", "parseBMP", "File too small to be a valid BMP: " + std::to_string(fileSize) + " bytes");
	}

	size_t rowStridePadded = (static_cast<size_t>(width) * 3 + 3) & ~static_cast<size_t>(3);
	size_t needed = static_cast<size_t>(dataOffset) + rowStridePadded * static_cast<size_t>(height);
	if (needed > fileSize) {
		delete[] buffer;
		return error("BMPParser", "parseBMP", "File too small for declared dimensions: " + std::to_string(fileSize) + " bytes, need " + std::to_string(needed) + " bytes");
	}

	bmpOut.fileSize = static_cast<uint32_t>(fileSize);
	bmpOut.fileHeader.offsetData = dataOffset;
	bmpOut.infoHeader.size       = dibSize;
	bmpOut.infoHeader.width      = width;
	bmpOut.infoHeader.height     = height;
	bmpOut.infoHeader.bitCount   = 24;
	bmpOut.infoHeader.xPixelsPerMeter = xppm;
	bmpOut.infoHeader.yPixelsPerMeter = yppm;
	bmpOut.infoHeader.colorsUsed      = colorsUsed;
	bmpOut.infoHeader.colorsImportant = colorsImportant;
	bmpOut.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 3);

	const unsigned char* srcPixels = buffer + dataOffset;
	for (int row = 0; row < height; ++row) {
		int srcRow = bottomUp ? (height - 1 - row) : row;
		const unsigned char* src = srcPixels + rowStridePadded * static_cast<size_t>(srcRow);
		unsigned char* dst = bmpOut.pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(width) * 3;

		for (int col = 0; col < width; ++col) {
			const size_t s = static_cast<size_t>(col) * 3;
			dst[s + 0] = src[s + 2];
			dst[s + 1] = src[s + 1];
			dst[s + 2] = src[s + 0];
		}
	}

	delete[] buffer;
	return true;
}