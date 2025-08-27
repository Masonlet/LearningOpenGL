#pragma once

#include "controller.hpp"
#include "objects/model.hpp"

struct ModelController : public Controller<Model> {
	void update(Model& data, const InputManager& input, float deltaTime) override;
};