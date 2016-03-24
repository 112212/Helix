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
    
    /*
    int depthSize;
    int stencilSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
    std::cout << "Depth buffer: " << depthSize << " Stencil buffer: " << stencilSize << std::endl;
    */

    //SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    //SDL_WarpMouseInWindow(window, sizeX / 2, sizeY / 2);
    //SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    //SDL_SetWindowGrab(window, SDL_TRUE);
    //SDL_ShowCursor(SDL_ENABLE);

    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    float mouseScroll = 0.0;
    
    he::Fps fps;
    
    he::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    
    he::Shader pyroShader("../src/Engine/Shaders/test_02.vs", "../src/Engine/Shaders/test_02.fs", "pyroShader");
    
    he::Model pyroModel("../src/Assets/Models/Pyro/Pyro.obj");
    
    SDL_Rect cursorRect;
    cursorRect.x = (sizeX / 2) - 8;
    cursorRect.y = (sizeY / 2) - 8;
    cursorRect.w = 17;
    cursorRect.h = 17;
    
    SDL_Surface* cursorSurface = IMG_Load("../src/Assets/Images/cursor_crosshair.png");
    if(!cursorSurface) {
        throw std::string("Error loading image: ") + IMG_GetError();
    }

    //moving this up he::Model is causing the texture bug? some texture binds error?
    SDL_Texture* cursorTexture = SDL_CreateTextureFromSurface(renderer, cursorSurface);

    auto t_start = std::chrono::high_resolution_clock::now();
    
    bool fullscreen = true;
    bool wireframe = true;
    
    bool running = true;

    while(running)
    {
        double dt = fps.GetDeltaTime();

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
                    
                    case SDLK_q:
                    {
                        static int display_in_use = 0; /* Only using first display */

                        int i, display_mode_count;
                        SDL_DisplayMode mode;
                        Uint32 f;

                        SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());

                        display_mode_count = SDL_GetNumDisplayModes(display_in_use);
                        if (display_mode_count < 1) {
                            throw std::string("SDL_GetNumDisplayModes failed ") + SDL_GetError();
                        }
                        SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);

                        for (i = 0; i < display_mode_count; ++i) {
                            if (SDL_GetDisplayMode(display_in_use, i, &mode) != 0) {
                                throw std::string("SDL_GetDisplayMode failed ") + SDL_GetError();
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
                            //SDL_SetWindowDisplayMode(window, 0);
                            sizeX = 1366;
                            sizeY = 768;
                            fullscreen = false;
                            //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
                            SDL_WarpMouseGlobal(sizeX / 2, sizeY / 2);
                        }
                        else {
                            SDL_SetWindowFullscreen(window, 0);
                            sizeX = 800;
                            sizeY = 600;
                            fullscreen = true;
                        }
                    }
                    break;
                    
                    case SDLK_e:
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
            else if(e.type == SDL_MOUSEWHEEL) {
                switch(e.wheel.type)
                {
                case SDL_MOUSEWHEEL:
                    mouseScroll = 0;
                    mouseScroll += e.wheel.y/10.0f;
                    //mouseScroll += e.wheel.y;
                    break;

                default:
                    break;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                    //xpos = e.motion.x;
                    //ypos = e.motion.y;
                    //std::cout << "x: " << xpos << " y: " << ypos << std::endl;
            }
        }

        SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
        
        SDL_PumpEvents();
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        if (state[SDL_SCANCODE_W]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::FORWARD, dt * 2.0);
        }
        
        if (state[SDL_SCANCODE_S]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::BACKWARD, dt * 2.0);
        }
        
        if (state[SDL_SCANCODE_A]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::LEFT, dt * 2.0);
        }
        
        if (state[SDL_SCANCODE_D]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::RIGHT, dt * 2.0);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        cursorRect.x = (sizeX / 2) - 8;
        cursorRect.y = (sizeY / 2) - 8;

        int xpos;
        int ypos;
        SDL_GetRelativeMouseState(&xpos, &ypos);
        
        camera.ProcessMouseMovement(xpos, ypos);
        camera.ProcessMouseScroll(mouseScroll);
        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), sizeX/(float)sizeY, 0.1f, 1000.0f);
        
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        
        glUseProgram(pyroShader.GetShader("pyroShader"));
        //remove argument [ pyroShader.GetShader(); ] ?
        pyroShader.InitPyro("pyroShader", time, view, projection);
        pyroModel.Draw(pyroShader.GetShader("pyroShader"));
        glUseProgram(0);
        
        SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
        
        SDL_RenderPresent(renderer);
        
        fps.Show();
        
        //SDL_Delay(16.0f - dt);
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
