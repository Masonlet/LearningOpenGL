#include "core/cameraManager.hpp"
#include "utils/log.hpp"

bool CameraManager::addCamera(const Camera& camera) {
	if (cameras.size() >= MAX_CAMERAS) return error("CameraManager", "addCamera", "max cameras reached");

	const std::string& name = camera.getName();
	if (cameras.find(name) != cameras.end()) return error("CameraManager", "addCamera", "name '" + name + "' already exists");

	cameras[name] = camera;
	if (cameras.size() == 1) activeCameraName = name;
	return true;
}

void CameraManager::setActiveCamera(unsigned int index) {
	if (index >= cameras.size()) {
		debugLog("CameraManager", "Invalid camera index : " + index, true);
		return;
	}

	std::map<std::string, Camera>::iterator it = cameras.begin();
	std::advance(it, index);
	activeCameraName = it->first;
}
Camera* CameraManager::getActiveCamera() {
	std::map<std::string, Camera>::iterator it = cameras.find(activeCameraName);
	if (it != cameras.end()) return &(it->second);
	else                     return nullptr;
}
int CameraManager::getCameraCount() {
	return cameras.size();
}

void CameraManager::updateActiveCamera(const Vec3& pos, 
	const float yaw, const float pitch,
	const float fov, const float nearPlane, const float farPlane) {

	Camera* cam = getActiveCamera();
	if (!cam) return;

	cam->setPos(pos);
	cam->setYaw(yaw);
	cam->setPitch(pitch);
	cam->setFov(fov);
	cam->setNear(nearPlane);
	cam->setFar(farPlane);
}