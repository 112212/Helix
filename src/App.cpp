#include "App.hpp"

#include <iostream>
#include <chrono>

namespace he = Helix;

App::App() {};
App::~App() {};

void App::Init()
{
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
 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }

    window = SDL_CreateWindow("Helix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, SDL_WINDOW_OPENGL);
    if(window == nullptr) {
        throw std::string("Failed to create window: ") + SDL_GetError();
    }
    
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr) {
        throw std::string("Failed to create GLContext: ") + SDL_GetError();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr) {
        throw std::string("Failed to create renderer: ") + SDL_GetError();
    }
    
    glewExperimental = GL_TRUE; 
    glewInit();
    
    int depthSize;
    int stencilSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
    
    std::cout << "Depth buffer: " << depthSize << " Stencil buffer: " << stencilSize << std::endl;
    
    //he::Gui* something = new he::Gui();
    //something->test();
        
    SDL_Surface* surface = IMG_Load("../src/Engine/Shaders/texture.jpg");
    if(!surface) {
        throw std::string("Error loading image: ") + IMG_GetError();
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    he::Fps fps;
    fps.Init();
    
    he::Shader shader;
    
    shader.LoadShader("../src/Engine/Shaders/test_01.vs", "../src/Engine/Shaders/test_01.fs", "testShader1");
    shader.Init(surface, "testShader1");
    
    auto t_start = std::chrono::high_resolution_clock::now();
    
    bool fullscreen = true;
    bool wireframe = true;
    
    bool running = true;
    
    while(running)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) {
                running = false;
            }
            else if(e.type == SDL_KEYDOWN) {
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
                    
                    case SDLK_a:
                    {
                        static int display_in_use = 0; /* Only using first display */

                        int i, display_mode_count;
                        SDL_DisplayMode mode;
                        Uint32 f;

                        SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());

                        display_mode_count = SDL_GetNumDisplayModes(display_in_use);
                        if (display_mode_count < 1) {
                            SDL_Log("SDL_GetNumDisplayModes failed: %s", SDL_GetError());
                            throw std::string("errr");
                        }
                        SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);

                        for (i = 0; i < display_mode_count; ++i) {
                            if (SDL_GetDisplayMode(display_in_use, i, &mode) != 0) {
                                SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
                                throw std::string("errr");
                            }
                            f = mode.format;

                            SDL_Log("Mode %i\tbpp %i\t%s\t%i x %i", i,
                            SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName(f), mode.w, mode.h);
                        }
                    }
                    break;
                    
                    case SDLK_f:
                    {
                        if(fullscreen) {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            fullscreen = false;
                        }
                        else {
                            SDL_SetWindowFullscreen(window, 0);
                            fullscreen = true;
                        }
                    }
                    break;
                    
                    case SDLK_w:
                    {
                        if(wireframe) {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            wireframe = false;
                        }
                        else {
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

        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
   
        shader.Draw(time);
                
        //SDL_GL_SwapWindow(window);

        SDL_RenderPresent(renderer);

        fps.Show();
        
        //SDL_Delay(16);
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
