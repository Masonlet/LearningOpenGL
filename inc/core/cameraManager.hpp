#pragma once

#include "core/camera.hpp"

class CameraManager {
public:
	static const unsigned int MAX_CAMERAS = 10;

	bool addCamera(const Camera& camera);

	void setActiveCamera(unsigned int index);
	Camera* getActiveCamera();
	int getCameraCount();

	void updateActiveCamera(
		const Vec3& position,
		const float yaw, const float pitch, 
		const float fov, const float nearPlane, const float farPlane
	);

	const std::map<std::string, Camera>& getAllCameras() const { return cameras; }

private:
	std::map<std::string, Camera> cameras;
	std::string activeCameraName;
};