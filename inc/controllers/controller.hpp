#pragma once

#include "core/inputManager.hpp"

template <typename T>
class Controller {
public:
	virtual ~Controller() = default;
	virtual void update(T& object, const InputManager& input, float deltaTime) = 0;
};