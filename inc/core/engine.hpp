#pragma once

// OpenGL and GLAD
#define GLFW_INCLUDE_NONE // Disables inclusion of the development environment header to ensure there are no header conflicts
#include <GLFW/glfw3.h> // Include GFLW API's constants, types, and function prototypes. By default also includes OpenGL header

#include <glad/glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

#include "core/colour.hpp"
#include "core/modelInstance.hpp"
#include "core/scene.hpp"
#include "core/sceneLoader.hpp"
#include "core/camera.hpp"

#include "graphics/shaderManager.hpp"
#include "graphics/vaoManager.hpp"
#include "graphics/renderer.hpp"

#include "lights/lightManager.hpp"

class Engine {
public:
	Engine();
	~Engine();

	inline bool getWireframe() const { return wireframe; }
	inline std::map<std::string, ModelInstance>& getModelInstances() { return scene.getModelInstances(); }
	inline LightManager* getLightManager() { return &lightManager; }
	inline Renderer* getRenderer() { return &renderer; }
	inline SceneLoader& getSceneLoader() { return sceneLoader; }

	void updateWireframe();
	void updateAspect(unsigned int width, unsigned int height);

	void run(const std::string& scene = "Default");

private:
	GLFWwindow* window;
	Camera camera;
	ShaderManager shaderManager;
	VAOManager vaoManager;
	LightManager lightManager;
	Renderer renderer;

	Scene scene;
	SceneLoader sceneLoader;

	unsigned int height, width;
	float aspect;
	bool wireframe;

	unsigned int currentProgram, currentModel, currentLight;
	float deltaTime, lastTime;

	void setupShaders();	
	void setupGLState();
	void setCallbacks();

	void updateDeltaTime(const float currentTime);
	void handleInputs();
	void renderFrame();
};
