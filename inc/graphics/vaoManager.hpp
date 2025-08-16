#pragma once

#include "math/vertex.hpp"
#include "math/mat4.hpp"
#include "core/colour.hpp"
#include "core/modelDrawInfo.hpp"
#include <string>
#include <map>

class VAOManager {
public:
	bool LoadPrimitiveIntoVAO(ModelDrawInfo& drawInfo, unsigned int shaderProgramID);
	bool LoadModelFromFile(const std::string& path, ModelDrawInfo& drawInfo);
	bool LoadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID);

	bool FindDrawInfoByModelName(const std::string& fileName, const ModelDrawInfo*& drawInfo);

	std::string GetLastError(bool bAndClear = true);

	void Shutdown();

private:
	std::map<std::string, ModelDrawInfo> modelName_to_VAOID;

	bool UploadToGPU(ModelDrawInfo& info, unsigned int shaderProgramID);

	std::string lastErrorString;
	void AppendTextToLastError(std::string text, bool addNewLineBefore = true);
};
