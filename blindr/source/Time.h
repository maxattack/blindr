#pragma once

namespace Time {
	extern int ticks;
	extern int deltaTicks;
	
	inline double seconds() { return 0.001 * ticks; }
	inline double deltaSeconds() { return 0.001 * deltaTicks; }
	
	void init();
	void tick();
}