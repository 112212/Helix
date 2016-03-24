#include "Fps.hpp"

//http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement

//move whole FPS to App.hpp?

namespace Helix {
    Fps::Fps()
    {
        m_tickPrevious = SDL_GetTicks();
        m_tickCurrent = 0;
        m_framesCurrent = 0;
        m_framesElapsed = 0;
        
        deltaTime = 0.0;
        thisTime = 0;
        lastTime = 0; 
    }
    
    Fps::~Fps() {}
    
    void Fps::Show()
    {
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
    
    double Fps::GetDeltaTime()
    {
        thisTime = SDL_GetTicks();
        deltaTime = (float)(thisTime - lastTime) / 1000;
        lastTime = thisTime; 
        
        return deltaTime;
    }
}

