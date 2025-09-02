#include "scene/sceneManager.hpp"
#include "starletparsers/utils/log.hpp"
#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>

bool SceneManager::loadTxtScene(const std::string& name) {
	debugLog("SceneManager", "loadTxtScene", "Start time: " + std::to_string(glfwGetTime()), true);

	if(!loader.loadScene(scene, name.c_str()))
		return error("SceneManager", "loadTxtScene", "Failed to load scene file: " + name);

	scene.setSceneName(name);
	return debugLog("SceneManager", "loadTxtScene", "Finish time:" + std::to_string(glfwGetTime()), true);
}

bool SceneManager::saveTxtScene() {
	debugLog("SceneManager", "saveTxtScene", "Start time: " + std::to_string(glfwGetTime()), true);

	if(!loader.saveScene(scene))
		return error("SceneManager", "saveTxtScene", "Failed to save scene file: " + scene.getSceneName());

	return debugLog("SceneManager", "saveTxtScene", "Finish time:" + std::to_string(glfwGetTime()), true);;
}