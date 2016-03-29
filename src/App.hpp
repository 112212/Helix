#include "Engine/Engine.hpp"

#include <chrono>

class App {
    public:
        App();
        ~App();
        
    private:
        void init();
        void loop(); 
        void showFPS();
        int getSizeX() const;
        int getSizeY() const;
        void setSizeX(int sizeX);
        void setSizeY(int sizeY);
        double getDeltaTime() const;
        double getTimeElapsed() const;
        void takeScreenshot(int x, int y, int w, int h);
        
        int m_sizeX;
        int m_sizeY;
        
        int m_ticks_previous;
        int m_ticks_current;
        int m_frames_current;
        int m_frames_elapsed;

        int m_ticks_then;
        double m_delta_time;
        
        std::chrono::high_resolution_clock::time_point m_chrono_start;
        double m_chrono_elapsed;
};  
