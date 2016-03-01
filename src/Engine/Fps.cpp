#include "Fps.hpp"

//http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement

namespace Helix {
	Fps::Fps() {};
	Fps::~Fps() {};
	
	void Fps::init() {
		interval = 1.0;

		tickPrevious = SDL_GetTicks();
		tickCurrent = 0;
		framesCurrent = 0;
		framesElapsed = 0;	
	}
	
	void Fps::show() {
		framesElapsed++;
		tickCurrent = SDL_GetTicks() - interval * 1000;
		
		if(tickPrevious < tickCurrent)
		{
			tickPrevious = SDL_GetTicks();
			framesCurrent = framesElapsed;
			framesElapsed = 0;
			
			std::cout << "FPS: " << framesCurrent << std::endl;
		}
	}
}

