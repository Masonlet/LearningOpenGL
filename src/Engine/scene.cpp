#include "scene.hpp"
#include "Mat4.hpp"

class DefaultDemo : public Scene {
	bool loaded{false};
	unsigned int count{1};

public:
	void Load(Engine& engine) override {
		struct TempModel {
			std::string name;
			std::string path;
			Mat4 transform;
		};

		const TempModel models[] = {
			// Rainbow 
			{ "ship", "Assets/Models/GalacticaOriginal_ASCII_no_text.ply", Mat4::translation(Vec3{0.0f, 0.0f, 100.0f})},

			// Green
			{ "car", "Assets/Models/de--lorean.ply", Mat4::translation(Vec3{0.0f, 0.0f, 10.0f})},

			// Blue
			{ "dolphin", "Assets/Models/dolphin.ply", Mat4::translation(Vec3{0.0f, -100.0f, -50.0f}) },

			//Yellow
			{ "audi", "Assets/Models/AUDI_TT_COUPE.ply", Mat4::translation(Vec3{-20.0f, 0.0f, 10.0f}) },

			//Random
			{ "homer", "Assets/Models/homer.ply", Mat4::translation(Vec3{-20.0f, 0.0f, 50.0f}) * Mat4::scale(Vec3{100.0f, 100.0f, 100.0f}) }
		};

		for (const TempModel& model : models) {
			engine.loadModel(model.name, model.path, model.transform);
		}

		loaded = true;
	}
	
	inline bool isLoaded() const { return loaded; }
	unsigned int objectCount() const { return count; }
};

Scene* createSceneFromName(const std::string& name) {
	if (name == "Default") return new DefaultDemo();

	return nullptr;
}