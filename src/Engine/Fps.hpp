#include <iostream>

#include <SDL2/SDL.h>

namespace Helix {
	class Fps {
		public:
			Fps();
			~Fps();
			
			void init();
			void show();
		private:
			double interval;
			
			double tickPrevious;
			double tickCurrent;
			
			uint framesCurrent;
			uint framesElapsed;
	};	
}
