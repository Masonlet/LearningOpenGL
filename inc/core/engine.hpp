#pragma once

#include "core/modelInstance.hpp"

#include "core/windowManager.hpp"
#include "core/cameraManager.hpp"
#include "core/inputManager.hpp"
#include "scene/sceneManager.hpp"

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
	inline SceneManager& getSceneManager() { return sceneManager; }
	inline WindowManager& getWindowManager() { return windowManager; }
	inline std::map<std::string, ModelInstance>& getModelInstances() { return getSceneManager().getScene().getModelInstances(); }

	inline bool getWireframe() const { return wireframe; }
	inline float getDeltaTime() const { return deltaTime; }

	void updateWireframe();

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool setScene(const std::string& sceneIn = "Default");
	void run();

	void incrementModel();
	void decrementModel();

private:
	WindowManager windowManager;
	Renderer renderer;
	ShaderManager shaderManager;
	VAOManager vaoManager;

	InputManager inputManager;
	CameraManager cameraManager;
	LightManager lightManager;	
	SceneManager sceneManager;

	bool wireframe;

	unsigned int currentProgram, currentModel;
	float deltaTime, lastTime;

	void setupShaders();	
	void tick(const float currentTime);
	void renderFrame();
};
