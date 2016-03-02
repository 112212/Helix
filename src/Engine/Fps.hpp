#include <iostream>

#include <SDL2/SDL.h>

namespace Helix {
    class Fps {
        public:
            Fps();
            ~Fps();
            
            void Init();
            void Show(); 
        private:
            double m_tickPrevious;
            double m_tickCurrent;
            
            uint m_framesCurrent;
            uint m_framesElapsed;
    };  
}
