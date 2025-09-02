#pragma once

#include "shader.hpp"
#include <vector>
#include <map>
#include <string>

class ShaderManager {
public:		
	ShaderManager() = default;
	~ShaderManager();

	bool useProgram(const std::string& name) const;
	unsigned int getProgramID(const std::string& name) const;

	bool createProgramFromPaths(const std::string& name, const std::string& vertPath, const std::string& fragPath);

	bool findShader(const std::string& name) const;
	bool getShader(const std::string& name, Shader*& dataOut);

private:
	bool compileShader(unsigned int& outShaderID, int glShaderType, const std::string& source);

	bool linkProgram(unsigned int& outProgramID, unsigned int vertID, unsigned int fragID);

	std::map<std::string, Shader> nameToShaders;
};
