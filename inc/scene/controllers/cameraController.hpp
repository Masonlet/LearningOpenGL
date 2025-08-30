#pragma once
#include "scene/objects/camera.hpp"
#include "scene/controllers/controller.hpp"

struct FreeCameraController : public Controller<Camera> {
	inline void setCamera(unsigned int cameraIndex, std::size_t cameraCount) { if (cameraIndex < cameraCount) current = cameraIndex; }

	void adjustFov(Camera& data, float fov);
	void print(const Camera& data) const;

	void update(Camera& data, const InputManager& input, float deltaTime) override;
};