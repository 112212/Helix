#include "App.hpp"

namespace he = Helix;

App::App()
{
    m_sizeX = 800;
    m_sizeY = 600;
    
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
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext glContext;
 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }

    window = SDL_CreateWindow("Helix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->getSizeX(), this->getSizeY(), SDL_WINDOW_OPENGL);
    if(window == nullptr) {
        throw std::string("Failed to create window: ") + SDL_GetError();
    }
    
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr) {
        throw std::string("Failed to create renderer: ") + SDL_GetError();
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
    
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr) {
        throw std::string("Failed to create GLContext: ") + SDL_GetError();
    }

    SDL_GL_SetSwapInterval(0);
    SDL_GL_MakeCurrent(window, glContext);
    
    glewExperimental = GL_TRUE; 
    glewInit();
    
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    printf("GLSL:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    int depthSize;
    int stencilSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
    std::cout << "Depth buffer: " << depthSize << " Stencil buffer: " << stencilSize << std::endl;

    //SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    //SDL_WarpMouseInWindow(window, this->getSizeX() / 2, this->getSizeY() / 2);
    //SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    //SDL_SetWindowGrab(window, SDL_TRUE);
    //SDL_ShowCursor(SDL_ENABLE);

    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    
    float mouseScroll = 0.0;

    he::Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
    he::Camera camera2(camera.GetPosition());
    
    /*
    he::Shader pyroShader("pyroShader", "../Assets/Shaders/test_02.vs", "../Assets/Shaders/test_02.fs");
    he::Shader bobShader("bobShader", "../Assets/Shaders/test_02.vs", "../Assets/Shaders/test_02.fs");
       
    he::Model pyroModel("../Assets/Models/Pyro/Pyro.obj");
    he::Model bobModel("../Assets/Models/guard/boblampclean.md5mesh");
    */
    
    he::Shader frustumShader("../Assets/Shaders/frustum.vs", "../Assets/Shaders/frustum.fs");
    
    he::Shader skeletalAnimShader("../Assets/Shaders/test_03.vs", "../Assets/Shaders/test_03.fs");
    he::Shader skeletalAnimShaderVisual("../Assets/Shaders/test_03_visual.vs", "../Assets/Shaders/test_03_visual.fs", "../Assets/Shaders/test_03_visual.gs");
    
    he::ModelLoader loader;
    
    he::Model test(skeletalAnimShader.GetShader());
    loader.LoadModel("../Assets/Models/guard/boblampclean.md5mesh", &test);
    //test.SetModelTrans(transformBob);
      
    he::Model test2(skeletalAnimShader.GetShader());
    loader.LoadModel("../Assets/Models/Pyro/Pyro.obj", &test2);
    //test2.SetModelTrans(glm::translate(test2.modelTrans, glm::vec3(0.0, -2.0, -2.0)));
    
    //add scale and rotate methods, and then after translation and/or rotation, scale by:
    //glm::vec3(0.07f, 0.07f, 0.07f)
    
    /*
    SDL_Rect cursorRect;
    cursorRect.x = (this->getSizeX() / 2) - 8;
    cursorRect.y = (this->getSizeY() / 2) - 8;
    cursorRect.w = 17;
    cursorRect.h = 17;
    
    SDL_Surface* cursorSurface = IMG_Load("../Assets/Images/cursor_crosshair.png");
    if(!cursorSurface) {
        throw std::string("Error loading image: ") + IMG_GetError();
    }
    
    SDL_Texture* cursorTexture = SDL_CreateTextureFromSurface(renderer, cursorSurface);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); //use after SDL_CreateTextureFromSurface to prevent texture bug

    SDL_FreeSurface(cursorSurface);
    */
    
    
    std::vector<glm::vec3> pointsPositions;
    std::vector<glm::vec3> pointsColors;
    for(int x = 0; x < 10; ++x) {
        for(int y = 0; y < 10; ++y) {
            for(int z = 0; z < 10; ++z) {
                pointsPositions.push_back(glm::vec3(x, y, z));
                pointsColors.push_back(glm::vec3(1.0, 0.0, 0.0));
            }
        }
    }
    
    GLfloat colorsz[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                         1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    };

    
    GLubyte indicesFrustum[] = {1, 0, 4,
                                1, 4, 5,
                                1, 5, 6,
                                1, 6, 2,
                                2, 6, 3,
                                3, 6, 7,
                                3, 7, 4,
                                3, 0, 4,
                                /*
                                0, 1, 2,
                                0, 2, 3,
                                4, 5, 6,
                                4, 6, 7,
                                */         
    };
    
    GLubyte indicesFrustumOutline[] = {0, 1, 1, 2, 2, 3, 3, 0,
                                       4, 5, 5, 6, 6, 7, 7, 4,
                                       0, 4, 1, 5, 2, 6, 3, 7,                    
    };
    
    glm::mat4 identityModel;
    
    glUseProgram(frustumShader.GetShader());
    GLuint vao, vbo, vbos;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);  
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &vbos);
    glBindVertexArray(0);
    glUseProgram(0);
            
    
    glm::mat4 identityModel2;
    identityModel = glm::translate(identityModel, glm::vec3(0.0f, 0.0f, 0.0f));

    glUseProgram(frustumShader.GetShader());
    GLuint vao2, vbo2, vbos2;
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glGenBuffers(1, &vbo2);
    glGenBuffers(1, &vbos2);
    glBindVertexArray(0);
    glUseProgram(0);
    

    bool mouserelative = true;
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
                    {
                        //camera.ToggleLockY();
                        glm::vec3 newPos = glm::vec3(camera2.GetPosition());
                        newPos.z -= 1.01;
                        camera2.SetPosition(newPos);
                    }
                    break;
                    
                    case SDLK_RIGHT:
                    {
                        //camera.ToggleLockY();
                        glm::vec3 newPos = glm::vec3(camera2.GetPosition());
                        newPos.x += 1.01;
                        camera2.SetPosition(newPos);
                    }
                    break;
                    
                    case SDLK_LEFT:
                    {
                        //camera.ToggleLockY();
                        glm::vec3 newPos = glm::vec3(camera2.GetPosition());
                        newPos.x -= 1.01;
                        camera2.SetPosition(newPos);
                    }
                    break;
                    
                    case SDLK_DOWN:
                    {
                        //camera.ToggleLockY();
                        glm::vec3 newPos = glm::vec3(camera2.GetPosition());
                        newPos.z += 1.01;
                        camera2.SetPosition(newPos);
                    }
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
                            
                            //glViewport(0, 0, 1024, 768);
                            //GLint viewport[4];
                            //glGetIntegerv(GL_VIEWPORT, viewport);
                            
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            this->setSizeX(w);
                            this->setSizeY(h);
                            
                            std::cout << this->getSizeX() << "x" << this->getSizeY() << std::endl;
                            
                            fullscreen = false;
                        }
                        else {
                            SDL_SetWindowFullscreen(window, 0);
                            //SDL_SetWindowDisplayMode(window, 0);

                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            this->setSizeX(800);
                            this->setSizeY(600);
                            
                            std::cout << this->getSizeX() << "x" << this->getSizeY() << std::endl;
                            
                            fullscreen = true;
                        }
                        
                        skip = 2;     
                    }
                    break;
                    
                    case SDLK_SPACE:
                    {
                        if(mouserelative) {
                            SDL_SetRelativeMouseMode(SDL_FALSE);
                            mouserelative = false;
                        }
                        else {
                            SDL_SetRelativeMouseMode(SDL_TRUE);
                            mouserelative = true;
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
                        //this->takeScreenshot(0, 0, this->getSizeX(), this->getSizeY());
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
        
        
        glViewport(0, 0, this->getSizeX(), this->getSizeY());
        
        //SDL_RenderSetLogicalSize(renderer, this->getSizeX(), this->getSizeY());
        
        SDL_PumpEvents();
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        if(state[SDL_SCANCODE_W]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::FORWARD, this->getDeltaTime() * 2.0);
        }
        
        if(state[SDL_SCANCODE_S]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::BACKWARD, this->getDeltaTime() * 2.0);
        }
        
        if(state[SDL_SCANCODE_A]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::LEFT, this->getDeltaTime() * 2.0);
        }
        
        if(state[SDL_SCANCODE_D]) {
            camera.ProcessKeyboard(he::Camera::MoveDirection::RIGHT, this->getDeltaTime() * 2.0);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        /*
        cursorRect.x = (this->getSizeX() / 2) - 8;
        cursorRect.y = (this->getSizeY() / 2) - 8;
        */
        
        int xpos;
        int ypos;
        SDL_GetRelativeMouseState(&xpos, &ypos);

        if(skip > 0 && (xpos != 0 || ypos != 0)) {
            skip--; 
        }
        else {
            if(mouserelative) {
                camera.ProcessMouseMovement(xpos, ypos);
                camera.ProcessMouseScroll(mouseScroll);
            }
            else {
                int xpos;
                int ypos;
                SDL_GetMouseState(&xpos, &ypos);
                
                //std::cout << "x: " << xpos << " y: " << ypos << std::endl;
                
                /*
                std::cout << "cam x: " << camera.GetPosition().x
                          << " y: " << camera.GetPosition().y
                          << " z: " << camera.GetPosition().z << std::endl;
                */
            }
        }
        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), this->getSizeX()/(float)this->getSizeY(), 0.1f, 1000.0f);

        glm::mat4 view2 = camera2.GetViewMatrix();
        glm::mat4 projection2 = glm::perspective(glm::radians(camera2.GetZoom()), this->getSizeX()/(float)this->getSizeY(), 0.1f, 100.0f);   
        
        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0, -2.0, -2.0));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
            
        /*
        glUseProgram(pyroShader.GetShader("pyroShader"));
        //remove argument [ pyroShader.GetShader(); ] ?
        pyroShader.InitPyro("pyroShader", this->getTimeElapsed(), camera.GetPosition(), view, projection);
        pyroModel.Draw(pyroShader.GetShader("pyroShader"));
        glUseProgram(0);
        glUseProgram(bobShader.GetShader("bobShader"));
        bobShader.InitBob("bobShader", this->getTimeElapsed(), camera.GetPosition(), view, projection, 4);
        bobModel.Draw(bobShader.GetShader("bobShader"));
        glUseProgram(0);
        */
        
        /*
        test.tick(this->getTimeElapsed());
        glUseProgram(skeletalAnimShaderVisual.GetShader());
        //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        test.Draw(model, view, projection);
        //glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glUseProgram(0);
        
        glUseProgram(skeletalAnimShader.GetShader());
        test.Draw(model, view, projection);
        glUseProgram(0);
        */

        glm::mat4 model2;
        model2 = glm::translate(model2, glm::vec3(4.0f, -2.0f, -20.0f));
        model2 = glm::rotate(model2, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
       
        glUseProgram(skeletalAnimShader.GetShader());
        //test2.Draw(model2, view, projection);
        glUseProgram(0);
        
        glm::mat4 model3;
        model3 = glm::translate(model3, glm::vec3(0.0f, 0.0f, 0.0f));
        model3 = glm::rotate(model3, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        glUseProgram(skeletalAnimShader.GetShader());
        //test2.Draw(model3, view, projection);
        glUseProgram(0);
        
        camera.ExtractFrustumPlanes(view, projection);
        camera2.ExtractFrustumPlanes(view2, projection2);
        
        /*
        if(camera2.PointInFrustum(glm::vec3(4.0f, -2.0f, -2.0f))) {
            std::cout << "intersecting!" << std::endl;
        } else {
            std::cout << "out of frustum!" << std::endl;
        }
        */
        
        for(int i = 0; i < pointsPositions.size(); ++i) {
            if(camera2.PointInFrustum(pointsPositions[i] * glm::mat3(identityModel))) {
                pointsColors[i] = glm::vec3(0.0, 1.0, 0.0);
            } else {
                pointsColors[i] = glm::vec3(1.0, 0.0, 0.0);
            }
        }
        
        glUseProgram(frustumShader.GetShader());
        glBindVertexArray(vao);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 8, &camera2.m_frustum_vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colorsz), colorsz, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "model"), 1, GL_FALSE, glm::value_ptr(identityModel));
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        //glUniform3f(glGetUniformLocation(frustumShader.GetShader(), "inColor2"), 0.0, 0.0, 1.0);
 
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, indicesFrustum);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, indicesFrustumOutline);
        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_BLEND);
        
        
        
        glUseProgram(frustumShader.GetShader());
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(vao2);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pointsPositions.size(), &pointsPositions[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbos2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pointsColors.size(), &pointsColors[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);    
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "model"), 1, GL_FALSE, glm::value_ptr(identityModel2));
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(frustumShader.GetShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));        

        glDrawArrays(GL_POINTS, 0, 1000);
        glBindVertexArray(0);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glUseProgram(0);
        
        //SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
        
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

int App::getSizeX() const
{
    return m_sizeX;
}

int App::getSizeY() const
{
    return m_sizeY;
}

void App::setSizeX(int sizeX)
{
    m_sizeX = sizeX;
}

void App::setSizeY(int sizeY)
{
    m_sizeY = sizeY;
}

double App::getDeltaTime() const
{ 
    return m_delta_time;
}

double App::getTimeElapsed() const
{
    return m_chrono_elapsed;
}

void App::takeScreenshot(int x, int y, int w, int h)
{
    // horrible due to opengl bottom left pixel data, needs to be flipped to use BMP, or use PNG file format
    std::stringstream ss;
    ss << "screenshot_" << std::time(0) << ".bmp";
    std::string screenshotFileName = ss.str();
                        
    unsigned char* pixelData = new unsigned char[w*h*4]; // 4 bytes for RGBA
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(x, y, w, h, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);

    SDL_Surface* sfc = SDL_CreateRGBSurfaceFrom(pixelData, w, h, 32, w*4, 0, 0, 0, 0);
    
    SDL_Surface* result = SDL_CreateRGBSurface(sfc->flags, sfc->w, sfc->h,
        sfc->format->BytesPerPixel * 8, sfc->format->Rmask, sfc->format->Gmask,
        sfc->format->Bmask, sfc->format->Amask);
        
    Uint8* pixels = (Uint8*) sfc->pixels;
    Uint8* rpixels = (Uint8*) result->pixels;
 
    Uint32 pitch = sfc->pitch;
    Uint32 pxlength = pitch*sfc->h;
 
    for(int line = 0; line < sfc->h; ++line) {
        Uint32 pos = line * pitch;
        memcpy(&rpixels[pos], &pixels[(pxlength-pos)-pitch], pitch);
    }
    
    SDL_SaveBMP(result, screenshotFileName.c_str());

    SDL_FreeSurface(sfc);
    SDL_FreeSurface(result);
    
    delete [] pixelData;
}
