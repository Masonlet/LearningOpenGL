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

	inline std::map<std::string, ModelInstance>& getModelInstances() { return scene.getModelInstances(); }
	inline LightManager* getLightManager() { return &lightManager; }
	inline SceneLoader& getSceneLoader() { return sceneLoader; }

	inline bool getWireframe() const { return wireframe; }
	inline float getDeltaTime() const { return deltaTime; }

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	bool initialize();
	void run(const std::string& sceneIn);

	void incrementModel();
	void decrementModel();

private:
	GLFWwindow* window;
	Camera camera;
	InputManager input;

	ShaderManager shaderManager;
	VAOManager vaoManager;
	Renderer renderer;

	LightManager lightManager;
	Scene scene;
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
