#pragma once

#include "shaderManager.hpp"
#include "vaoManager.hpp"
#include "camera.hpp"

struct ModelInstance {
	std::string path;
	Mat4 transform;
};

class Engine {
public:
	Engine();
	~Engine();

	bool getWireframe() const { return wireframe; }
	unsigned int getProgram() const { return program; }
	VAOManager* getMeshManager() const { return meshManager; }

	bool loadModel(const std::string& name, const std::string& path, const Mat4& transform);
	void addModelInfo(const std::string& name, const ModelDrawInfo& info);
	bool addInstance(const std::string& name, const std::string& path, const Mat4& transform);

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	void run(const std::string& scene = "Default");

private:
	GLFWwindow* window;
	Camera camera;
	ShaderManager* shaderManager;
	VAOManager* meshManager;
		
	std::map<std::string, ModelInstance> modelInstances;	
	std::map<std::string, ModelDrawInfo> modelInfos;

	unsigned int height, width;
	unsigned int program, vertex_buffer;
	unsigned int currentProgram;
	unsigned int mvpLocation;
	float deltaTime, lastTime, aspect;
	bool wireframe;

	void setupShaders();	
	void setupGLState();
	void setCallbacks();

	bool drawModel(const std::string& name, const Mat4& view, const Mat4& projection);

	void updateDeltaTime(const float currentTime);
	void handleInputs();
	void renderFrame();

	void unloadModel(ModelDrawInfo& info);
	void clearModels();
};