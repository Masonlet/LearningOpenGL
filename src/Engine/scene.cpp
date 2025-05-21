#include "scene.hpp"
#include "generator.hpp"
#include "parser.hpp"
#include <cstdio>

class DefaultDemo : public Scene {
	bool loaded{false};
	unsigned int count{1};

public:
	void Load(Model* models) override {
		if (!models[0].mesh)
			models[0].mesh = new Mesh();
		if (!createTriangle(*models[0].mesh)) {
			fprintf(stderr, "Failed to create DefaultDemo triangle");
			loaded = false;
		}

		loaded = true;
	}
	
	inline bool isLoaded() const { return loaded; }
	unsigned int objectCount() const { return count; }
	RenderMode renderMode() const override { return RenderMode::basic3D; }
};
class CubeDemo : public Scene {
	bool loaded{false};
	const unsigned int count{10000};

public:
	void Load(Model* models) override {
		constexpr Vec3 rotation{0.0f, 0.0f, 0.0f};
		constexpr Vec3 scale{0.25f, 0.25f, 0.25f};
		constexpr Vec2 spacing{1.0f * scale.x, 1.0f * scale.y};

		if (!createCubeGrid(models, 0, count, spacing, rotation, scale)) {
			fprintf(stderr, "Failed to create PlaneDemo squareGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
	RenderMode renderMode() const override { return RenderMode::basic3D; }
};
class SpaceDemo : public Scene {
	bool loaded{false};
	const unsigned int count{10};

public:
	void Load(Model* models) override {
		const char* path = "Assets/Models/GalacticaOriginal_ASCII_no_text.ply";
		constexpr bool hasNormals{false};
		constexpr Vec3 rotation{90.0f, 0.0f, 0.0f};
		constexpr Vec3 scale{0.1f, 0.1f, 0.1f};
		constexpr Vec2 spacing{100.0f, 50.0f};

		if (!createModelGrid(models, path, 0, count, spacing, rotation, scale, hasNormals)) {
			fprintf(stderr, "Failed to create SpaceDemo modelGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
	RenderMode renderMode() const override { return RenderMode::basic3D; }
};
class CarDemo : public Scene {
	bool loaded{false};
	const unsigned int count{2500};

public:
	void Load(Model* models) override {
		const char* path = "Assets/Models/AUDI_TT_COUPE.ply";
		constexpr bool hasNormals{false};
		constexpr Vec3 rotation{0.0f, 0.0f, 0.0f};
		constexpr Vec3 scale{0.05f, 0.05f, 0.05f};
		constexpr Vec2 spacing{300.0f * scale.x, 630.0f * scale.y};

		if (!createModelGrid(models, path, 0, count, spacing, rotation, scale, hasNormals)) {
			fprintf(stderr, "Failed to create SpaceDemo modelGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
	RenderMode renderMode() const override { return RenderMode::basic3D; }
};
class basic2DPlaneDemo : public Scene {
	bool loaded{false};
	const unsigned int count{2500};

public:
	void Load(Model* models) override {
		constexpr Vec3 rotation{0.0f, 0.0f, 0.0f};
		constexpr Vec2 scale{0.1f, 0.1f};
		constexpr Vec2 spacing{1.0f * scale.x, 1.0f * scale.y};

		if (!createSquareGrid(models, 0, count, spacing, rotation, scale)) {
			fprintf(stderr, "Failed to create PlaneDemo squareGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
	RenderMode renderMode() const override { return RenderMode::basic2D; }
};

Scene* createSceneFromName(const char* name) {
	if (strEquals(name, "Default")) return new DefaultDemo();
	else if (strEquals(name, "Cube")) return new CubeDemo();
	else if (strEquals(name, "Space")) return new SpaceDemo();
	else if (strEquals(name, "Car")) return new CarDemo();
	else if (strEquals(name, "2DPlane")) return new basic2DPlaneDemo();

	return nullptr;
}