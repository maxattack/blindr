#include <SDL.h>
#include "Blindr.h"
#include <SDL_mixer.h>

using namespace Blindr;

int main(int argc, char ** argv)
{
	//Request OpenGL 3.2 context.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	// start up SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) { return -1; }
	
	Time::init();
	Graphics::init(3, 426, 240);
	SpriteBatch::init();
	VectorGraphics::init();
	Audio::init();
	
	
	Blindr::init();
	
	// play summadat muzik
	Mix_Music *music = Mix_LoadMUS("Space Camel.ogg");
	if (music) {
		Mix_PlayMusic(music, -1);
	} else {
		LOG(("Error: %s", Mix_GetError()));
	}
	
	Blindr::world->run(false);
	delete Blindr::world;
	for(;;) {
		Blindr::world = new World();
		Blindr::world->run(true);
		delete Blindr::world;
	}
	
	Audio::release();
	//VectorGraphics::release();
	//SpriteBatch::release();
	Graphics::release();
	//Time::release();
	SDL_Quit();
	return 0;
}
