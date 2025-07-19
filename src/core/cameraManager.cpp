#include "core/cameraManager.hpp"

CameraManager::CameraManager() : activeCameraIndex(0) {}

bool CameraManager::addCamera(const Camera& camera) {
	if (cameras.size() >= MAX_CAMERAS) {
		fprintf(stderr, "[CameraManager ERROR] Max cameras reached, cannot add camera\n");
		return false;
	}

	cameras.push_back(camera);
	return true;
}

void CameraManager::setActiveCamera(unsigned int index) {
	if (index < cameras.size() && index >= 0) 
		activeCameraIndex = index;
	else printf("[CameraManager WARN] No active camera set or active camera invalid.\n");
}
Camera* CameraManager::getActiveCamera() {
	return &cameras[activeCameraIndex];
}
int CameraManager::getCameraCount() {
	return cameras.size();
}

void CameraManager::updateActiveCamera(const Vec3& position, const Vec3& front, const Vec3& up,
	const float yaw, const float pitch,
	const float fov, const float aspect,
	const float nearPlane, const float farPlane) {
	Camera& camera = *getActiveCamera();
}