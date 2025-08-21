#pragma once

#include "models/modelData.hpp"
#include "graphics/renderer.hpp"
#include "core/windowManager.hpp"
#include "core/cameraManager.hpp"
#include "core/inputManager.hpp"
#include "scene/sceneManager.hpp"
#include "graphics/shaderManager.hpp"
#include "graphics/meshManager.hpp"
#include "graphics/textureManager.hpp"
#include "lights/lightManager.hpp"

class Engine {
public:
	Engine();
	~Engine();

	inline WindowManager& getWindowManager() { return windowManager; }
	inline Renderer* getRenderer() { return &renderer; }
	inline TextureManager* getTextureManager() { return &textureManager; }
	inline CameraManager* getCameraManager() { return &cameraManager; }
	inline LightManager* getLightManager() { return &lightManager; }
	inline SceneManager& getSceneManager() { return sceneManager; }
	inline std::map<std::string, ModelData>& getModelData() { return getSceneManager().getScene().getModelData(); }

	inline bool getWireframe() const { return wireframe; }
	inline float getDeltaTime() const { return deltaTime; }

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool setScene(const std::string& sceneIn = "Default");
	bool loadSceneMeshes();
	void run();

	void updateWireframe();

	void incrementModel();
	void decrementModel();

private:
	WindowManager windowManager;
	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;
	InputManager inputManager;
	CameraManager cameraManager;
	LightManager lightManager;	
	SceneManager sceneManager;

	bool wireframe;
	unsigned int currentProgram, currentModel;
	float deltaTime, lastTime;

	bool setupShaders();	

	void tick(const float currentTime);
	void renderFrame();
};
