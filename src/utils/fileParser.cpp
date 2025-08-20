#include "utils/fileParser.hpp"

constexpr size_t MAX_SIZE = static_cast<size_t>(200 * 1024) * 1024; //200MB Limit

static bool getFileSize(FILE* file, size_t& sizeOut) {
	// Seek to end 
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "[getFileSize] Failed to seek end of file\n");
		return false;
	}

	// Get the file size
	long size = ftell(file);
	if (size == -1L) {
		fprintf(stderr, "[getFileSize] Invalid file, ftell failed\n");
		return false;
	}

	if (size <= 0 || static_cast<size_t>(size) > MAX_SIZE) {
		fprintf(stderr, "[getFileSize] Invalid file size\n");
		return false;
	}

	if (fseek(file, 0, SEEK_SET) != 0) { 
		fprintf(stderr, "[getFileSize] rewind failed\n"); 
		return false; 
	}

	sizeOut = static_cast<size_t>(size);
	return true;
}

bool loadFile(std::string& out, const std::string& path) {
	// Open File
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "[loadFile] Failed to open file: %s\n", path.c_str());
		return false;
	}	

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fprintf(stderr, "[loadFile] Failed to get file size\n"); 
		fclose(file); 
		return false;
	}

	out.resize(fileSize);

	size_t bytesRead = 0;
	while (bytesRead < fileSize) {
		size_t byteRead = fread(&out[bytesRead], 1, fileSize - bytesRead, file);
		if (byteRead == 0) {
			if (ferror(file)) {
				fprintf(stderr, "[loadFile] fread failed at byte %zu\n", bytesRead);
				fclose(file);
				out.clear();
				return false;
			}
			break;
		}
		bytesRead += byteRead;
	}

	fclose(file);

	if (bytesRead != fileSize) {
		fprintf(stderr, "[loadFile] fread failed. Expected %zu bytes, got %zu\n", fileSize, bytesRead);
		out.clear();
		return false;
	}

	return true;
}

bool loadBinaryFile(const unsigned char*& dataOut, size_t& sizeOut, const std::string& path) {
	// Open File
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "[loadBinaryFile] Failed to open file: %s\n", path.c_str());
		dataOut = nullptr;
		sizeOut = 0;
		return false;
	}

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fprintf(stderr, "[loadBinaryFile] Failed to get file size\n"); 
		fclose(file); 
		dataOut = nullptr;
		sizeOut = 0;
		return false;
	}

	// Read file into buffer
	unsigned char* buffer = new unsigned char[fileSize];
	size_t bytesRead = 0;
	while(bytesRead < fileSize) {
		size_t byteRead = fread(buffer + bytesRead, 1, fileSize - bytesRead, file);
		if (byteRead == 0) {
			if (ferror(file)) {
				fprintf(stderr, "[loadBinaryFile] fread failed at byte %zu\n", bytesRead);
				delete[] buffer;
				fclose(file);
				dataOut = nullptr;
				sizeOut = 0;
				return false;
			}
			break;
		}
		bytesRead += byteRead;
	}
	fclose(file);

	if (bytesRead != fileSize) {
		fprintf(stderr, "[loadBinaryFile] Short read. Expected %zu, got %zu\n", fileSize, bytesRead);
		delete[] buffer; dataOut = nullptr; sizeOut = 0; return false;
	}

	if (dataOut) delete[] dataOut;
	dataOut = buffer;
	sizeOut = fileSize;
	return true;
}
