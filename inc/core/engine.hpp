#pragma once

#include "graphics/shaderManager.hpp"
#include "graphics/vaoManager.hpp"
#include "lights/lightManager.hpp"
#include "camera.hpp"

struct ModelInstance {
	ModelInstance() : colourMode(ColourMode::Solid){};

	Vec4 position;
	Vec3 rotation;
	Vec3 scale;
	Vec4 colour;

	Mat4 modelMatrix;

	std::string path;
	ColourMode colourMode;
};

class Engine {
public:
	Engine();
	~Engine();

	bool getWireframe() const { return wireframe; }
	unsigned int getProgram() const { return program; }
	VAOManager* getMeshManager() const { return meshManager; }

	bool loadModel(const std::string& name, const std::string& path, const Vec4& position, const Vec3& rotation, const Vec3& scale, const Vec4& assignedColour, ColourMode colourMode);
	void addModelInfo(const std::string& name, const ModelDrawInfo& info);
	bool addInstance(const std::string& name, const std::string& path, const Mat4& transform);

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	void run(const std::string& scene = "Default");

	void incrementModel();
	void decrementModel();

	bool saveScene();

private:
	GLFWwindow* window;
	Camera camera;
	ShaderManager* shaderManager;
	VAOManager* meshManager;
	LightManager* lightManager;
		
	std::map<std::string, ModelInstance> modelInstances;	
	std::map<std::string, ModelDrawInfo> modelInfos;

	unsigned int height, width;
	unsigned int program;
	unsigned int currentProgram, currentModel, currentLight;
	std::string currentScene;
	int modelLocation, modelViewLocation, modelProjectionLocation, modelInverseTransposeLocation, useOverrideColourLocation, colourOverrideLocation;
	float deltaTime, lastTime, aspect;
	bool wireframe;

	void setupShaders();	
	void setupGLState();
	void setCallbacks();

	bool createSceneFromName(const std::string& scene);

	bool drawModel(const ModelInstance& instance, const Mat4& view, const Mat4& projection);

	void updateDeltaTime(const float currentTime);
	void handleInputs();
	void renderFrame();

	void unloadModel(ModelDrawInfo& info);
	void clearModels();
};
