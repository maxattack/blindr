
#include "Blindr.h"
#include "DebugDraw.h"

GameAssets *Blindr::assets = 0;
Blindr::World *Blindr::world = 0;

void Blindr::init() {
	{
		void* data;
		size_t len;
		Assets::load("assets", &data, &len);
		Blindr::assets = (GameAssets*) data;
	}
	
	Blindr::world = new World();
	
}