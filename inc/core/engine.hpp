#pragma once

#include "objects/model.hpp"
#include "graphics/renderer.hpp"
#include "core/windowManager.hpp"
#include "core/inputManager.hpp"
#include "core/sceneManager.hpp"
#include "graphics/shaderManager.hpp"
#include "graphics/meshManager.hpp"
#include "graphics/textureManager.hpp"

struct Engine {
	WindowManager windowManager;
	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;
	InputManager inputManager;
	SceneManager sceneManager;

	bool wireframe{ false };
	unsigned int currentModel{ 0 };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool setScene(const std::string& sceneIn = "default") { return sceneManager.loadTxtScene(sceneIn); }
	bool loadSceneAssets();

	void run();

	void incrementModel();
	void decrementModel();

private:
	bool setupShaders();
	bool loadSceneMeshes();
	bool loadSceneTextures();

	void tick(const float currentTime);
	void renderFrame();
};
