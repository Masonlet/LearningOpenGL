#pragma once

#include "engine.hpp"

void handleModelInput(Camera& camera, GLFWwindow* window, float deltaTime, std::map<std::string, ModelInstance>& instances, int currentModel);
