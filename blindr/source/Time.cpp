#include "Time.h"
#include "SDL.h"

int Time::ticks = 0;
int Time::deltaTicks = 0;
	
void Time::init() {
	ticks = SDL_GetTicks();
}

void Time::tick() {
	deltaTicks = SDL_GetTicks() - ticks;
	ticks += deltaTicks;
}
