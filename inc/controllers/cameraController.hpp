#pragma once
#include "objects/camera.hpp"
#include "controllers/controller.hpp"

struct FreeCameraController : public Controller<Camera> {
	inline void setCamera(unsigned int cameraIndex, std::size_t cameraCount) { if (cameraIndex < cameraCount) current = cameraIndex; }

	void update(Camera& data, const InputManager& input, float deltaTime) override;
};