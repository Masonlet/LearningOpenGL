#pragma once

#include "core/windowManager.hpp"
#include "core/colour.hpp"
#include "core/modelInstance.hpp"
#include "core/camera.hpp"
#include "core/inputManager.hpp"

#include "scene/scene.hpp"
#include "scene/sceneLoader.hpp"

#include "graphics/shaderManager.hpp"
#include "graphics/vaoManager.hpp"
#include "graphics/renderer.hpp"

#include "lights/lightManager.hpp"

class Engine {
public:
	Engine();
	~Engine();

	inline Renderer* getRenderer() { return &renderer; }
	inline CameraManager* getCameraManager() { return &cameraManager; }
	inline LightManager* getLightManager() { return &lightManager; }
	inline SceneLoader& getSceneLoader() { return sceneLoader; }
	inline std::map<std::string, ModelInstance>& getModelInstances() { return getSceneLoader().getScene().getModelInstances(); }

	inline bool getWireframe() const { return wireframe; }
	inline float getDeltaTime() const { return deltaTime; }

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	bool initialize();
	bool setScene(const std::string& sceneIn = "Default");
	void run();

	void incrementModel();
	void decrementModel();

private:
	GLFWwindow* window;
	Renderer renderer;
	ShaderManager shaderManager;
	VAOManager vaoManager;

	InputManager inputManager;
	CameraManager cameraManager;
	LightManager lightManager;	
	SceneLoader sceneLoader;

	unsigned int height, width;
	float aspect;
	bool wireframe;

	unsigned int currentProgram, currentModel;
	float deltaTime, lastTime;

	void setupShaders();	

	void tick(const float currentTime);
	void renderFrame();
};
