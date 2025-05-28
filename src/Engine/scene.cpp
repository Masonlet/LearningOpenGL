#include "scene.hpp"
#include "Mat4.hpp"
#include "factory.hpp"

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
};
class CubeDemo : public Scene {
	bool loaded{false};
	const unsigned int count{5000};

public:
	void Load(Engine& engine) override {
		constexpr Vec3 rotation{0.0f, 0.0f, 0.0f};
		constexpr Vec3 scale{0.25f, 0.25f, 0.25f};
		constexpr Vec2 spacing{1.0f * scale.x, 1.0f * scale.y};

		if (!createCubeGrid(engine, "cube", 0, count, spacing, rotation, scale)) {
			fprintf(stderr, "Failed to create CubeDemo cubeGrid\n");
			loaded = false;
			return;
		}

		loaded = true;
	}

	inline bool isLoaded() const override { return loaded; }
};

Scene* createSceneFromName(const std::string& name) {
	if (name == "Default") return new DefaultDemo();
	else if (name == "Cube") return new CubeDemo();

	return nullptr;
}