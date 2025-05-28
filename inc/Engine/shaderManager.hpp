#pragma once

#include "vertex.hpp"
#include <vector>
#include <map>
#include <string>

class ShaderManager {
public:
	class Shader {
	public:
		unsigned int ID;
		std::vector<std::string> vecSource;
		bool sourceIsMultiLine;
		std::string fileName;

		enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, UNKNOWN };
		ShaderType type;

		Shader();
		~Shader();

		std::string getType();
	};

	class ShaderProgram {
	public:
		ShaderProgram() : ID(0) {};
		~ShaderProgram() {};

		unsigned int ID;
		std::string friendlyName;
	};

	ShaderManager() {};
	~ShaderManager() {};

	bool useShaderProgram(unsigned int ID);
	bool useShaderProgram(std::string friendlyName);
	bool createProgramFromFile(std::string friendlyName, Shader& vertexShader, Shader& fragShader);

	void setBasePath(const std::string& path);
	Shader makeShader(const std::string& path);

	int getIDFromFriendlyName(std::string friendlyName);
	ShaderProgram* getShaderProgramFromFriendlyName(std::string friendlyName);

	std::string getLastError();

private:
	bool loadSourceFromFile(Shader& shader) const;
	std::string basePath;

	bool compileShaderFromSource(ShaderManager::Shader& shader, std::string& error);
	bool wasThereACompileError(unsigned int shaderID, std::string& error);
	bool wasThereALinkError(unsigned int progID, std::string& error);

	std::string lastError;

	std::map<unsigned int, ShaderProgram> id_to_shader;
	std::map<std::string, unsigned int> name_to_id;
};