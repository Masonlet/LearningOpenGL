#pragma once

#include "core/inputManager.hpp"
#include "core/modelInstance.hpp"
#include <map>

void handleModelInput(InputManager* input, float deltaTime, std::map<std::string, ModelInstance>& instances, int currentModel);
