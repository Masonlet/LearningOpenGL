#pragma once

#include "shaderManager.hpp"
#include "vaoManager.hpp"
#include "camera.hpp"

struct ModelInit {
	const char* path;
	Mat4 transform;
};

class Engine {
public:
	Engine();
	~Engine();

	bool getWireframe() const { return wireframe; }

	bool loadModel(const std::string& path, const Mat4& transform);

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	void run();

private:
	GLFWwindow* window;
	Camera camera;
	ShaderManager* shaderManager;
	VAOManager* meshManager;
		
	std::map<std::string, Mat4> modelTransforms;	
	std::map<std::string, ModelDrawInfo> modelInfos;

	unsigned int height, width;
	unsigned int program, vertex_buffer;
	unsigned int currentProgram;
	unsigned int mvpLocation;
	float deltaTime, lastTime, aspect;
	bool wireframe;

	void setupShaders();	
	void setCallbacks();

	bool drawModel(const std::string& name, const Mat4& view, const Mat4& projection);

	void updateDeltaTime(const float currentTime);
	void handleInputs();
	void renderFrame();

	void unloadModel(ModelDrawInfo& info);
	void clearModels();
};