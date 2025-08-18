#include "textures/bmpParser.hpp"
#include "textures/bmp.hpp"
#include "utils/fileManager.hpp"
#include <cstdio>

static bool readHeader(const unsigned char* buffer, int& height, int& width) {
	if (!buffer) return false;

	if (buffer[0] != 'B' || buffer[1] != 'M') {
		std::fprintf(stderr, "[BMP Parser] Bad signature (not BM)\n");
		return false;
	}

	unsigned int dibSize =
		(unsigned int)buffer[14] |
		((unsigned int)buffer[15] << 8) |
		((unsigned int)buffer[16] << 16) |
		((unsigned int)buffer[17] << 24);
	if (dibSize < 40U) {
		std::fprintf(stderr, "[BMP Parser] Unsupported DIB header size: %u\n", dibSize);
		return false;
	}

	int32_t w = static_cast<int32_t>(
		static_cast<unsigned int>(buffer[18]) |
		(static_cast<unsigned int>(buffer[19]) << 8) |
		(static_cast<unsigned int>(buffer[20]) << 16) |
		(static_cast<unsigned int>(buffer[21]) << 24)
	);
	if (w <= 0) {
		std::fprintf(stderr, "[BMP Parser] Invalid width: %d\n", static_cast<int>(w));
		return false;
	}

	int32_t h = static_cast<int32_t>(
		static_cast<unsigned int>(buffer[22]) |
		(static_cast<unsigned int>(buffer[23]) << 8) |
		(static_cast<unsigned int>(buffer[24]) << 16) |
		(static_cast<unsigned int>(buffer[25]) << 24)
	);
	if (h == 0) {
		std::fprintf(stderr, "[BMP Parser] Invalid height: %d\n", static_cast<int>(h));
		return false;
	}

	unsigned short planes = static_cast<unsigned short>(
		static_cast<unsigned int>(buffer[26]) |
		(static_cast<unsigned int>(buffer[27]) << 8)
	);
	if (planes != 1) {
		std::fprintf(stderr, "[BMP Parser] Planes != 1: %u\n", (unsigned)planes);
		return false;
	}

	unsigned short bpp = (unsigned short)(
		static_cast<unsigned int>(buffer[28]) |
		(static_cast<unsigned int>(buffer[29]) << 8)
	);
	if (bpp != 24) {
		std::fprintf(stderr, "[BMP Parser] Only 24bpp supported: %u\n", (unsigned)bpp);
		return false;
	}

	unsigned int compression =
		static_cast<unsigned int>(buffer[30]) |
		(static_cast<unsigned int>(buffer[31]) << 8) |
		(static_cast<unsigned int>(buffer[32]) << 16) |
		(static_cast<unsigned int>(buffer[33]) << 24
	);
	if (compression != 0) {
		std::fprintf(stderr, "[BMP Parser] Compressed BMP not supported: %u\n", compression);
		return false;
	}

	width = static_cast<int>(w);
	height = (h > 0) ? static_cast<int>(h) 
									 : static_cast<int>(-h); 
	return true;
}

bool parseBMP(const char* filename, BMP& bmpOut) {
	unsigned char* buffer{ nullptr };
	size_t fileSize{ 0 };

	if (!loadBinaryFile(buffer, fileSize, filename) || !buffer) return false;
	if (fileSize < 54) {
		std::fprintf(stderr, "[BMP Parser] File too small\n");
		delete[] buffer;
		return false;
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
		std::fprintf(stderr, "[BMP Parser] Bad data offset\n");
		delete[] buffer;
		return false;
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

	if (dataOffset >= fileSize) {
		std::fprintf(stderr, "[BMP Parser] Bad data offset\n");
		delete[] buffer;
		return false;
	}

	size_t rowStridePadded = (static_cast<size_t>(width) * 3 + 3) & ~static_cast<size_t>(3);
	size_t needed = static_cast<size_t>(dataOffset) + rowStridePadded * static_cast<size_t>(height);
	if (needed > fileSize) {
		std::fprintf(stderr, "[BMP Parser] File too small for declared dimensions\n");
		delete[] buffer;
		return false;
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

	size_t outSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
	unsigned char* rgb = new unsigned char[outSize];

	const unsigned char* srcPixels = buffer + dataOffset;
	for (int row = 0; row < height; ++row) {
		int srcRow = bottomUp ? (height - 1 - row) : row;
		const unsigned char* src = srcPixels + rowStridePadded * static_cast<size_t>(srcRow);
		unsigned char* dst = rgb + static_cast<size_t>(row) * static_cast<size_t>(width) * 3;

		for (int col = 0; col < width; ++col) {
			unsigned char b = src[col * 3 + 0];
			unsigned char g = src[col * 3 + 1];
			unsigned char r = src[col * 3 + 2];
			dst[col * 3 + 0] = r;
			dst[col * 3 + 1] = g;
			dst[col * 3 + 2] = b;
		}
	}

	delete[] buffer;
	bmpOut.data = rgb;
	return true;
}