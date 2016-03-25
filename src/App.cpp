#include "App.hpp"

namespace he = Helix;

App::App()
{
    m_ticks_previous = SDL_GetTicks();
    m_ticks_current = 0;
    m_frames_current = 0;
    m_frames_elapsed = 0;
    
    m_delta_time = 0;
    m_ticks_then = 0;

    m_chrono_start = std::chrono::high_resolution_clock::now();
    m_chrono_elapsed = 0;
    
    this->init();
}

App::~App() {}

void App::init()
{
    int sizeX = 800;
    int sizeY = 600;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    //SDL_GLContext glContext;
 
 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }

    window = SDL_CreateWindow("Helix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, SDL_WINDOW_OPENGL);
    if(window == nullptr) {
        throw std::string("Failed to create window: ") + SDL_GetError();
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetSwapInterval(1);
    
    /*
    SDL_GLContext glContext;
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr) {
        throw std::string("Failed to create GLContext: ") + SDL_GetError();
    }
    */
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr) {
        throw std::string("Failed to create renderer: ") + SDL_GetError();
    }
    
    //SDL_GL_MakeCurrent(window, glContext);
    
    glewExperimental = GL_TRUE; 
    glewInit();
    
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    printf("GLSL:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    
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

    he::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    
    he::Shader pyroShader("pyroShader", "../Assets/Shaders/test_02.vs", "../Assets/Shaders/test_02.fs");
    he::Shader bobShader("bobShader", "../Assets/Shaders/test_02.vs", "../Assets/Shaders/test_02.fs");
       
    he::Model pyroModel("../Assets/Models/Pyro/Pyro.obj");
    he::Model bobModel("../Assets/Models/guard/boblampclean.md5mesh");

    SDL_Rect cursorRect;
    cursorRect.x = (sizeX / 2) - 8;
    cursorRect.y = (sizeY / 2) - 8;
    cursorRect.w = 17;
    cursorRect.h = 17;
    
    SDL_Surface* cursorSurface = IMG_Load("../Assets/Images/cursor_crosshair.png");
    if(!cursorSurface) {
        throw std::string("Error loading image: ") + IMG_GetError();
    }

    SDL_Texture* cursorTexture = SDL_CreateTextureFromSurface(renderer, cursorSurface);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); //use after SDL_CreateTextureFromSurface to prevent texture bug

    SDL_FreeSurface(cursorSurface);
    
    bool fullscreen = true;
    bool wireframe = true;
    
    int skip = 0;
    
    bool running = true;

    while(running)
    {
        this->loop();

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
                            
                            int w, h;
                            SDL_GetRendererOutputSize(renderer, &w, &h);
                            sizeX = w;
                            sizeY = h;
                            fullscreen = false;
                            
                            std::cout << sizeX << "x" << sizeY << std::endl;
                        }
                        else {
                            SDL_SetWindowFullscreen(window, 0);
                            //SDL_SetWindowDisplayMode(window, 0);
                            
                            fullscreen = true;

                            sizeX = 800;
                            sizeY = 600;
                            
                            std::cout << sizeX << "x" << sizeY << std::endl;
                        }
                        
                        skip = 2;     
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
                    
                    case SDLK_p:
                    {
                        std::stringstream ss;
                        ss << "screenshot_" << std::time(0) << ".bmp";
                        std::string screenshotFileName = ss.str();
                        
                        SDL_Surface* screenshotSurface = SDL_CreateRGBSurface(0, sizeX, sizeY, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshotSurface->pixels, screenshotSurface->pitch);
                        SDL_SaveBMP(screenshotSurface, screenshotFileName.c_str());
                        SDL_FreeSurface(screenshotSurface);
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
            camera.ProcessKeyboard(he::Camera::MoveDirection::FORWARD, this->getDeltaTime() * 2.0);
        }
        
        if (state[SDL_SCANCODE_S]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::BACKWARD, this->getDeltaTime() * 2.0);
        }
        
        if (state[SDL_SCANCODE_A]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::LEFT, this->getDeltaTime() * 2.0);
        }
        
        if (state[SDL_SCANCODE_D]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::RIGHT, this->getDeltaTime() * 2.0);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        cursorRect.x = (sizeX / 2) - 8;
        cursorRect.y = (sizeY / 2) - 8;

        int xpos;
        int ypos;
        SDL_GetRelativeMouseState(&xpos, &ypos);
        
        if(skip > 0 && (xpos != 0 || ypos != 0)) {
            skip--; 
        }
        else {
            camera.ProcessMouseMovement(xpos, ypos);
            camera.ProcessMouseScroll(mouseScroll);
        }
        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), sizeX/(float)sizeY, 0.1f, 1000.0f);
        
        glUseProgram(pyroShader.GetShader("pyroShader"));
        //remove argument [ pyroShader.GetShader(); ] ?
        pyroShader.InitPyro("pyroShader", this->getTimeElapsed(), view, projection);
        pyroModel.Draw(pyroShader.GetShader("pyroShader"));
        glUseProgram(0);

        glUseProgram(bobShader.GetShader("bobShader"));
        bobShader.InitBob("bobShader", this->getTimeElapsed(), view, projection, 4);
        bobModel.Draw(bobShader.GetShader("bobShader"));
        glUseProgram(0);       

        SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
        
        //SDL_RenderPresent(renderer);
        SDL_GL_SwapWindow(window);

        this->showFPS();
        
        //SDL_Delay(16);
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}

void App::loop()
{
    // fps
    m_frames_elapsed++;
    m_ticks_current = SDL_GetTicks();
    
    // delta time
    m_delta_time = (m_ticks_current - m_ticks_then) / 1000.0f;
    m_ticks_then = m_ticks_current;

    // time elapsed
    auto m_chrono_now = std::chrono::high_resolution_clock::now();
    m_chrono_elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(m_chrono_now - m_chrono_start).count();
}

void App::showFPS()
{
    if(m_ticks_previous < m_ticks_current - 1000) {
        m_ticks_previous = SDL_GetTicks();
        m_frames_current = m_frames_elapsed;
        m_frames_elapsed = 0;
        
        if(m_frames_current < 1) {
            m_frames_current = 1;
        }
        
        std::cout << "FPS: " << m_frames_current << std::endl;
    }
}

double App::getDeltaTime()
{ 
    return m_delta_time;
}

double App::getTimeElapsed()
{
    return m_chrono_elapsed;
}
