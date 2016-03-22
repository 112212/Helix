#include "Engine/Engine.hpp"

#include <iostream>
#include <chrono>

namespace he = Helix;
 
int main(int argc, char* args[])
{
    int posX = 100;
    int posY = 200;
    int sizeX = 800;
    int sizeY = 600;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext glContext;
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    SDL_GL_SetSwapInterval(1);
 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("Helix", posX, posY, sizeX, sizeY, SDL_WINDOW_OPENGL);
    if(window == nullptr)
    {
        std::cout << "Failed to create window : " << SDL_GetError();
        return -1;
    }
    
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr)
    {
        std::cout << "Failed to create GLContext : " << SDL_GetError();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr)
    {
        std::cout << "Failed to create renderer : " << SDL_GetError();
        return -1;
    }
    
    glewExperimental = GL_TRUE; 
    glewInit();
    
    int depthSize;
    int stencilSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
    
    std::cout << "Depth buffer: " << depthSize << " Stencil buffer: " << stencilSize << std::endl;
    
    SDL_Rect playerPos;
    playerPos.x = 20;
    playerPos.y = 20;
    playerPos.w = 20;
    playerPos.h = 20;
    
    //he::Gui* something = new he::Gui();
    //something->test();
    
        
    SDL_Surface* surface = IMG_Load("../src/Engine/Shaders/texture.jpg");
    if(!surface) {
        std::cout << "Error loading image: " << IMG_GetError() << std::endl;
        return -1;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    he::Fps fps;
    fps.Init();
    
    he::Shader shader;
    
    try {
        shader.Initialize();
        shader.Init(surface);
    } catch(std::string& error) {
        std::cout << error << std::endl;
        return -1;
    }
    
    auto t_start = std::chrono::high_resolution_clock::now();
    
    bool wireframe = true;
    
    bool running = true;
    
    while(running)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
            else if(e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_UP:
                        std::cout << "up" << std::endl;
                    break;

                    case SDLK_DOWN:
                        std::cout << "down" << std::endl;
                    break;

                    case SDLK_LEFT:
                        std::cout << "left" << std::endl;
                    break;

                    case SDLK_RIGHT:
                        std::cout << "right" << std::endl;
                    break;
                    
                    case SDLK_w:
                    {
                        if(wireframe) {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            wireframe = false;
                        }
                        else
                        {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                            wireframe = true;
                        }
                    }
                    break;
                    
                    case SDLK_ESCAPE:
                        running = false;
                    break;

                    default:
                    break;
                }
            }
        }
        
        SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);

        /*
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderFillRect(renderer, &playerPos);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        
        SDL_RenderPresent(renderer);
        */
        
        //SDL_GL_SwapWindow(window);
        
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

        try {
            shader.Draw(time);
        } catch(std::string& error) {
             std::cout << error << std::endl;
             return -1;
        }
                
        //SDL_GL_SwapWindow(window);
        
        SDL_RenderPresent(renderer);

        fps.Show();
        
        //SDL_Delay(16);
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
