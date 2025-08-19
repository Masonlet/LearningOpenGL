#ifdef NDEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <cstdlib>
#endif

#include "core/engine.hpp"
#include "textures/bmpParser.hpp"
#include "textures/bmp.hpp"

int main() {
#ifdef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc();
#endif
	
	Engine engine{};
	if (!engine.initialize(1920, 1200, "Test"))
		return -1;

	if (!engine.setScene("Maze"))
		return -1;

	if (!engine.loadSceneMeshes())
		return -1;

	//engine.getTextureManager()->SetPath("assets/textures");
	//engine.getTextureManager()->Create2DBMPTexture("Lava_Texture.bmp", true);
	//std::map<std::string, ModelData>& objects = engine.getModelData();
	//ModelData* lavaModel = &objects["cow3"];
	//lavaModel->textureNames[0] = "Lava_Texture.bmp";
	//lavaModel->textureMixRatio[0] = 1.0f;
	//lavaModel->useTextures = true;

	engine.run();
	return 0;
}
