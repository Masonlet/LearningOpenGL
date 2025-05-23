#include "texture.hpp"
#include "files.hpp"
#include "parser.hpp"
#include <cstdio>

static bool ensure(int condition, const char* errorMsg) {
	if (condition) return true;
     
	fprintf(stderr, "Invalid file: %s\n", errorMsg);
	return false;
}

static bool checkHeader(unsigned char* buffer) {
	constexpr unsigned char PNG_HEADER[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	for (int i = 0; i < 8; i++) { 
		if (!ensure(static_cast<unsigned char>(buffer[i]) == PNG_HEADER[i], "Header error"))
			return false;
	}

	return true;
}

static void takeChunkType(unsigned char* buffer, unsigned char* out) {
	for (int i = 0; i < 4; i++)
		out[i] = buffer[i];
	out[4] = '\0';
}


/*for (size_t i = 0; i < deflateSize; ++i) {
	printf("%02X\n", deflateData[i]);
}*/

static bool validateHeader(unsigned char* idatData, size_t idatSize, unsigned int& CMF, unsigned int& FLG, unsigned char*& deflateData, size_t& deflateSize) {
	if (idatSize < 6) return ensure(false, "IDAT chunk too small to contain valid ZLIB header");
	
	CMF = idatData[0];
	FLG = idatData[1];

	unsigned int CM = CMF & 0x0F; //bits 0 to 3 -- compression method
	unsigned int CINFO = (CMF >> 4) & 0X0F; //bits 4 to 7 -- compression info

	if (!ensure(CM == 8, "Unsupported compression method (only deflate supported)")) return false;

	// Supports a window size up to 32K, method used by gzip and PNG
	//CINFO is the base-2 logarithm of the LZ77 window size, minus eight (CINFO = 7 indicates a 32K window size)
	if (!ensure(CINFO <= 7, "Invalid window size")) return false;
	
	//bits 0 to 4 -- FCHECK (Check bits for CMF and FLG)
	//(CMF*256 + FLG) must be a multiple of 31
	if (!ensure((((CMF << 8) | FLG) % 31 == 0), "FCHECK failed")) return false;

	//bits 5 -- FDICT (preset dictionary)
	unsigned int FDICT = (FLG >> 5) & 1;
	
	//bits 6 to 7 -- FLEVEL (compression level), not needed for decompression

	//n byte (actual compressed data)
	deflateData = idatData + 2;
	deflateSize = idatSize - 2 - 4; // 2-byte zlib header + 4-byte Adler32

	if (FDICT) {
		deflateData += 4;
		deflateSize -= 4;
	}

	return true;
}

static unsigned int parseBits(unsigned char*& cursor, unsigned char* deflateData, size_t deflateSize, size_t& bitPos, int bitCount) {
	unsigned int result = 0;

	for (int i = 0; i < bitCount; ++i) {
		if ((cursor - deflateData) >= deflateSize) {
			fprintf(stderr, "Reached EOF\n");
			return result;
		}

		result |= ((cursor[0] >> bitPos) & 1) << i;

		++bitPos;
		if (bitPos == 8) {
			bitPos = 0;
			++cursor;
		}
	}

	return result;
}

/* while (true) {
*	read header from input stream (shown below)
* 
*   if stored block (Bit 00 (0))  
*		skip bits until byte aligned
*		read count and 1's compliment of count
*		copy count bytes data block
*   else
*		loop until end of block code sent (Bit 0)
*			decode literal character from input stream
*			if literal < 256
*				copy character to the output stream
*			otherwise
*				if literal = end of block
*					break;
*				otherwise
*					decode distance from input stream
*					move backwards distance bytes in the output stream, and copy length characters from this position to the output stream
*		end loop
*  }
*/
/* Compressed Header Data
* Bit 0: Last Block bit (Set to 1 if this is the last compressed block in the data)
*
* Bit 1-2: Block Type
*	00 (0) - Block is stored
*		All stored data is byte aligned
*		Skip bits until next byte, then next word = block length,
*		followed by the ones compliment of the block length word.
*		Remaining data in the block is the stored data.
*
*	01 (1) - Use fixed Huffman codes for literal and distance codes
*		Literal_Code Bits Distance_Code Bits
*		0 - 143      8    0 - 31        5
*		144 - 255    9
*		256 - 279    7
*		280 - 287    8                      
*		Literal codes 286 -287 and distance codes 30-31 are never used, but participate in the huffman construction.
*
*	10 (2) - Dynamic Huffman Codes 
*		Dynamic Huffman codes are sent in the following compressed format,
*		5 Bits: # of Literal Codes sent - 256 (256 - 286), All other codes are never sent      
*		5 Bits: # of Dist codes - 1 (1 - 32)
*		4 Bits: # of Bit Length codes - 3 (3 - 19)
*
*		The bit lengths themselves are compressed with Huffman codes. There are 19 bit length codes  
*		0 - 15: Represent bit lengths of 0 - 15
*		16: Copy the previous bit length 3 - 6 times.
*			The next 2 bits indicate repeat length (0 = 3, ... , 3 = 6)
*			Example: Codes 8, 16 (+2 bits 11), 16 (+2 bits 10) will expand to 12 bit lengths of 8 (1 + 6 + 5)
*		17: Repeat a bit length of 0 for 3 - 10 times. (3 bits of length)
*		18: Repeat a bit length of 0 for 11 - 138 times. (7 bits of length)   
*
*		The lengths of the bit length codes are sent packed 3 bits per value (0 - 7) in the following order,
*		16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
*
*		These codes are sent as bit lengths and the codes are built as described in the implode algorithm
*		EXCEPT codes that are assigned starting at the shortest bit length, i.e. the shorted could SHOULD be all 0's rather than all 1's
*		Codes with a bit length of zero do not participate in the tree construction. 
*		The codes are then used to decode the bit lengths for the literal and distance tables.
* 
*		The bit lengths for the literal tables are sent first with the number of entries sent described by the 5 bits sent earlier
*		There are up to 286 literal characters; 
*			The first 256 represent the respective 8 bit character,
*			Code 256 represents the End-Of-Block code,
*			The remaining 29 codes represent copy lengths of 3 thru 258.
*			There are up to 30 distance codes representing distances from 1 thru 32k as described below.
* 
*		Length Codes
*		Extra            Extra            Extra            Extra
*		Code Bits Length Code Bits Length Code Bits Length Code Bits Length(s)
*		257  0    3      265  1    11,12  273  3    35-42  281  5    131-162
*		258  0    4		 266  1    13,14  274  3	43-50  282  5	 163-194
*		259  0	  5		 267  1	   15,16  275  3	51-58  283  5	 195-226
*		260  0	  6		 268  1	   17,18  276  3	59-66  284  5    227-257
*		261  0	  7		 269  2	   19-22  277  4	67-82  285  0    258
*		262  0	  8		 270  2	   23-26  278  4	83-98  
*		263  0	  9		 271  2	   27-30  279  4	99-114
*		264  0	  10	 272  2	   31-40  280  4	115-130
* 
*		Distance Codes
*		Extra              Extra              Extra               Extra
*		Code Bits Distance Code Bits Distance Code Bits Distance  Code Bits Distance
*		0    0	  1		   8	3	 17-24	  16   7	257-384   24   11   4097-6144
*		1	 0	  2		   9    3    25-32	  17   7	385-512   25   11	6145-8192
*		2    0	  3		   10	4    33-48	  18   8	513-768   26   12   8193-12288
*		3    0	  4		   11   4    49-64	  19   8	769-1024  27   12   12289-16384
*		4    1	  5,6	   12   5    65-96	  20   9	1025-1536 28   13   16385-24576
*		5    1	  7,8	   13   5    97-128   21   9	1537-2048 29   13   24577-32768
*		6    2	  9-12	   14   6    129-192  22   10	2049-3072 
*		7    2	  13-16	   15   6    193-256  23   10   3073-4096 
* 
*	11 (3) - Reserved - Flag a "Error in compressed data" if seen */

static unsigned char* parseIdatStream(unsigned char* idatData, size_t idatSize, size_t expected_output_size) {
	unsigned int CMF, FLG;
	unsigned char* deflateData;
	unsigned char* outData = new unsigned char[expected_output_size];
	size_t deflateSize, writePos{0};

	if (!validateHeader(idatData, idatSize, CMF, FLG, deflateData, deflateSize))
		return nullptr;

	//Loop through all deflate blocks
	unsigned char* cursor = deflateData;
	size_t bitPos = 0; // 0 -- 7

	while (true) {
		unsigned int b0 = parseBits(cursor, deflateData, deflateSize, bitPos, 1);

		unsigned int b1 = parseBits(cursor, deflateData, deflateSize, bitPos, 1);
		unsigned int b2 = parseBits(cursor, deflateData, deflateSize, bitPos, 1);

		//Bit 00 (Stored Block)
		if (b1 == 0 && b2 == 0) {
			if (bitPos != 0) {
				bitPos = 0;
				++cursor;
			}

			if ((cursor - deflateData) + 4 > deflateSize) {
				fprintf(stderr, "Not enough bytes for LEN/NLEN in stored block\n");
				delete[] outData;
				return nullptr;
			}

			//Block can be 'raw data', 'LZ77-compressed data encoded with fixed Huffman codes', or 'LZ77-compressed data encoded with custom huffman codes'
			unsigned int len = cursor[0] | (cursor[1] << 8); //# of uncompressed bytes right after it
			unsigned int nlen = cursor[2] | (cursor[3] << 8); //#bitwise compliment of len (used to check for corrupt data)

			if ((len ^ 0xFFFF) != nlen) {
				fprintf(stderr, "LEN/NLEN mismatch\n");
				delete[] outData;
				return nullptr;
			}

			cursor += 4;

			// Raw data here
			if ((cursor - deflateData) + len > deflateSize) {
				 fprintf(stderr, "Not enough bytes for stored data\n");
				 delete[] outData;
				 return nullptr;
			}

			for (unsigned i = 0; i < len; ++i) {
				outData[writePos++] = cursor[i];
			}

			cursor += len;
			
			// If `b0 == 1`, this was the final block
			if (b0 == 1)
				break;
		}

		//Bit 01 (Fixed Huffman Codes for literal and distance codes)
		else if (b1 == 0 && b2 == 1) {

		}

		//Bit 10 (Dynamic Huffman Codes)
		else if (b1 == 1 && b2 == 0) {

		}

		//Bit 11 (Reserved)
		else if (b1 == 1 && b2 == 1) {
			fprintf(stderr, "Encountered reserved block type\n");
			delete[] outData;
			return nullptr;
		} 
	}

	//4 bytes (check value, algorithm used is called Adler32)
	//Any data after is not apart of the stream

	return outData;
}

static size_t calculateExpectedSize(unsigned int width, unsigned int height, unsigned int bitDepth, unsigned int colourType) {
	size_t bytesPerPixel;

	switch (colourType) {
		case 0: // Grayscale
			bytesPerPixel = static_cast<size_t>((bitDepth + 7) / 8.0);
			break;
		case 2: // RGB
			bytesPerPixel = static_cast<size_t>(3.0 * ((bitDepth + 7) / 8.0));
			break;
		case 3: // Indexed-color
			bytesPerPixel = static_cast<size_t>(1);
			break;
		case 4: // Grayscale + Alpha
			bytesPerPixel = static_cast<size_t>(2.0 * ((bitDepth + 7) / 8.0));
			break;
		case 6: // RGBA
			bytesPerPixel = static_cast<size_t>(4.0 * ((bitDepth + 7) / 8.0));
			break;
		default:
			return 0;
	}

	size_t rowSize = 1 + width * bytesPerPixel; // +1 for PNG filter byte
	return height * rowSize;
}
unsigned char* Texture::parsePNG(unsigned char* buffer, size_t fileSize) {
	if (!checkHeader(buffer))
		return nullptr;

	unsigned char* cursor = buffer + 8;
	const unsigned char* end = buffer + (fileSize - 8);

	size_t idatSize = 0;
	unsigned char* idatData = nullptr;

	// Parse every chunk
	while (cursor + 12 <= end) {
		if (cursor + 8 > end) {
			fprintf(stderr, "Unexpected EOF while reading chunk header\n");
			break;
		}
		
		unsigned int length = parseBinaryUINT(cursor);
		cursor += 4;

		unsigned char chunkType[5];
		takeChunkType(cursor, chunkType);
		cursor += 4;

		if (cursor + length + 4 > end) {
			fprintf(stderr, "Chunk exceeds file bounds\n");
			return nullptr;
		}

		if (strEquals(chunkType, "IHDR")) {
			if (length != 13) {
				fprintf(stderr, "Invalid IHDR chunk length: %u", length);
				return nullptr;
			}

			width = parseBinaryUINT(cursor);
			cursor += 4;

			height = parseBinaryUINT(cursor);
			cursor += 4;

			bitDepth = cursor[0]; //bit depth (1 byte, values 1, 2, 3, 4, 8, or 16)
			colourType = cursor[1]; //colour type (1 byte, values 0, 2, 3, 4, or 6)
			compression = cursor[2]; //compression method (1 byte, value 0)
			filter = cursor[3]; //filter method (1 byte, value 0)
			interlace = cursor[4]; // interlace method (1 byte, values 0 "no interface" or 1 "Adam7" interface) (?? 13 data bytes total ??)
			cursor += 5;

			printf("%u %u %u %u %u %u %u\n", width, height, bitDepth, colourType, compression, filter, interlace);
		} 
		else if (strEquals(chunkType, "iCCP")) {
			cursor += length;
		}
		else if (strEquals(chunkType, "pHYs")){
			cursor += length;
		}
		else if (strEquals(chunkType, "IDAT")) {
			unsigned char* newBuf = new unsigned char[idatSize + length]; 
			
			if (idatData) {
				for (unsigned int i = 0; i < idatSize; ++i)
					newBuf[i] = idatData[i];
				delete[] idatData;
			}
			for (unsigned int i = 0; i < length; ++i)
				newBuf[idatSize + i] = cursor[i];

			idatData = newBuf;
			idatSize += length;
			cursor += length;
		}
		else if (strEquals(chunkType, "IEND")) {
			cursor += length;
			return cursor;
		}
		else {
			fprintf(stderr, "Chunk Not Implemented: %.4s\n", chunkType);
			cursor += length;
		}

		//Skip CRC
		cursor += 4;
	}

	if (idatData && idatSize >= 2) {
		size_t expected_output_size = calculateExpectedSize(width, height, bitDepth, colourType);
		if (expected_output_size == 0) {
			fprintf(stderr, "Unsupported color type or bit depth\n");
			delete[] idatData;
			return nullptr;
		}

		parseIdatStream(idatData, idatSize, expected_output_size);
	} else {
		fprintf(stderr, "No valid IDAT chunk found or IDAT size is too small.\n");
		delete[] idatData;
		return nullptr;
	}

	return cursor;
}

bool Texture::load(const char* path) {
	unsigned char* buffer{};
	size_t fileSize;

	if (!loadFile(buffer, path, fileSize)) {
		fprintf(stderr, "Failed to load file: %s\n", path);
		return false;
	}

	unsigned char* cursor = parsePNG(buffer, fileSize);
		
	if (!cursor) {
		return false;
	}

	printf("Success");
	return true;
}