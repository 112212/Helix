#include "Fps.hpp"

//http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement

namespace Helix {
    Fps::Fps() {};
    Fps::~Fps() {};
    
    void Fps::Init() {
        m_tickPrevious = SDL_GetTicks();
        m_tickCurrent = 0;
        m_framesCurrent = 0;
        m_framesElapsed = 0;    
    }
    
    void Fps::Show() {
        m_framesElapsed++;
        m_tickCurrent = SDL_GetTicks() - 1.0 * 1000;
        
        if(m_tickPrevious < m_tickCurrent)
        {
            m_tickPrevious = SDL_GetTicks();
            m_framesCurrent = m_framesElapsed;
            m_framesElapsed = 0;
            
            if(m_framesCurrent < 1)
            {
                m_framesCurrent = 1;
            }
            
            std::cout << "FPS: " << m_framesCurrent << std::endl;
        }
    }
}

