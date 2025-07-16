#pragma once

#include "core/camera.hpp"

class CameraManager {
public:
	static const unsigned int MAX_CAMERAS = 10;

	CameraManager();

	bool addCamera(const Camera& camera);

	void setActiveCamera(unsigned int index);
	Camera* getActiveCamera();
	int getCameraCount();

	void updateActiveCamera(
		const Vec3& position, const Vec3& front, const Vec3& up,
		const float yaw, const float pitch,
		const float fov, const float aspect,
		const float nearPlane, const float farPlane
	);

private:
	std::vector<Camera> cameras;
	unsigned int activeCameraIndex;
};