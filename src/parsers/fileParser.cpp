#include "parsers/fileParser.hpp"
#include "utils/log.hpp"

constexpr size_t MAX_SIZE = static_cast<size_t>(200 * 1024) * 1024; //200MB Limit
static bool getFileSize(FILE* file, size_t& sizeOut) {
	if (fseek(file, 0, SEEK_END) != 0) return error("FileParser", "getFileSize", "Failed to seek end of file");
	
	long size = ftell(file);
	if (size == -1L) return error("FileParser", "getFileSize", "Invalid file, ftell failed");
	
	if (size <= 0 || static_cast<size_t>(size) > MAX_SIZE) 
		return error("FileParser", "getFileSize", "Invalid file size");
	
	if (fseek(file, 0, SEEK_SET) != 0) return error("FileParser", "getFileSize", "Failed to rewind file");

	sizeOut = static_cast<size_t>(size);
	return true;
}

bool loadFile(std::string& out, const std::string& path) {
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) return error("FileParser", "loadFile", "Failed to open file: " + path);

	size_t fileSize;
	if (!getFileSize(file, fileSize)) return error("FileParser", "loadFile", "Failed to get file size");

	out.resize(fileSize);

	size_t bytesRead = 0;
	while (bytesRead < fileSize) {
		size_t byteRead = fread(&out[bytesRead], 1, fileSize - bytesRead, file);
		if (byteRead == 0) {
			if (ferror(file)) {
				fclose(file);
				out.clear();
				return error("FileParser", "loadFile", "fread failed at byte " + std::to_string(bytesRead));
			}
			break;
		}
		bytesRead += byteRead;
	}

	fclose(file);

	if (bytesRead != fileSize) {
		out.clear();
		return error("FileParser", "loadFile", "fread failed. Expected " + std::to_string(fileSize) + ", got " + std::to_string(bytesRead));
	}
	return true;
}

bool loadBinaryFile(const unsigned char*& dataOut, size_t& sizeOut, const std::string& path) {
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		dataOut = nullptr;
		sizeOut = 0;
		return error("FileParser", "loadBinaryFile", "Failed to open file: " + path);
		}

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fclose(file); 
		dataOut = nullptr;
		sizeOut = 0;
		return error("FileParser", "loadBinaryFile", "Failed to get file size");
	}

	unsigned char* buffer = new unsigned char[fileSize];
	size_t bytesRead = 0;
	while(bytesRead < fileSize) {
		size_t byteRead = fread(buffer + bytesRead, 1, fileSize - bytesRead, file);
		if (byteRead == 0) {
			if (ferror(file)) {
				delete[] buffer;
				fclose(file);
				dataOut = nullptr;
				sizeOut = 0;
				return error("FileParser", "loadBinaryFile", "fread failed at byte " + std::to_string(bytesRead));
			}
			break;
		}
		bytesRead += byteRead;
	}
	fclose(file);

	if (bytesRead != fileSize) {
		delete[] buffer; 
		dataOut = nullptr;
		sizeOut = 0; 
		return error("FileParser", "loadBinaryFile", "fread failed. Expected " + std::to_string(fileSize) + ", got " + std::to_string(bytesRead));
	}

	if (dataOut) delete[] dataOut;
	dataOut = buffer;
	sizeOut = fileSize;
	return true;
}
