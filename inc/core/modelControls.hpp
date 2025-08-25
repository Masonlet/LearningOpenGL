#pragma once

#include "core/inputManager.hpp"
#include "objects/model.hpp"
#include <map>

void handleModelInput(InputManager* input, float deltaTime, std::map<std::string, Model>& instances, int currentModel);
