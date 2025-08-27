#pragma once

#include "controller.hpp"
#include "objects/model.hpp"

class ModelController : public Controller<Model> {
	public:

	unsigned int currentModel{ 0 };
	inline void incrementModel(std::size_t modelCount) { if (modelCount != 0 && static_cast<size_t>(currentModel + 1) < modelCount) currentModel++; }
	inline void decrementModel() { if (currentModel != 0) currentModel--; }

	void update(Model& model, const InputManager& input, float deltaTime) override;
};