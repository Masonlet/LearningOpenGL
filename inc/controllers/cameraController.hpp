#pragma once
#include "objects/camera.hpp"
#include "controllers/controller.hpp"

class FreeCameraController : public Controller<Camera> {
public:

	unsigned int currentCamera{ 0 };
	inline void setCamera(unsigned int cameraIndex, std::size_t cameraCount) { if (cameraIndex < cameraCount) currentCamera = cameraIndex; }
	inline void incrementCamera(std::size_t cameraCount) { if (cameraCount - 1 > currentCamera) currentCamera++; }
	inline void decrementCamera() { if (currentCamera != 0) currentCamera--; }

	void update(Camera& camera, const InputManager& input, float deltaTime) override;
};