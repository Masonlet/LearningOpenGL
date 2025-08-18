#pragma once

#include "core/inputManager.hpp"
#include "models/modelData.hpp"
#include <map>

void handleModelInput(InputManager* input, float deltaTime, std::map<std::string, ModelData>& instances, int currentModel);
