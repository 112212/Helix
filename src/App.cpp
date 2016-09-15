#include "App.hpp"

#include "Engine/commands/commands.hpp"

#include "controls/TextBox.hpp"

int get_config_value(std::string key) {
	try {
		return Command::Get(key);
	} catch(...) {
		return 0;
	}
}


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
    SDL_GLContext glContext;
 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }

    window = SDL_CreateWindow("Helix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->getSizeX(), this->getSizeY(), SDL_WINDOW_OPENGL);
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
    
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr) {
        throw std::string("Failed to create GLContext: ") + SDL_GetError();
    }

    SDL_GL_SetSwapInterval(0);
    SDL_GL_MakeCurrent(window, glContext);
    
    glewExperimental = GL_TRUE; 
    GLenum glew = glewInit();
    if(GLEW_OK != glew) {
        throw std::string("Failed to initialize GLEW");
    }
    
    std::cout << "Vendor:     " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:   " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version:    " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL:       " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    /*
    int depthSize;
    int stencilSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
    std::cout << "Depth buffer: " << depthSize << " Stencil buffer: " << stencilSize << std::endl;
    */

    //SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    //SDL_WarpMouseInWindow(window, this->getSizeX() / 2, this->getSizeY() / 2);
    //SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    //SDL_SetWindowGrab(window, SDL_TRUE);
    //SDL_ShowCursor(SDL_ENABLE);
    
    // ng::Drawing::SetResolution(this->getSizeX(), this->getSizeY());
    // ng::Drawing::Init();
    
    he::Engine* engine = he::Engine::Instance();
    //engine->Init();
    
    engine->gui = new ng::GuiEngine(this->getSizeX(), this->getSizeY());
    engine->designerGui = new ng::GuiEngine(this->getSizeX(), this->getSizeY());
    
    engine->gui->SetSize(this->getSizeX(), this->getSizeY());
    engine->gui->SetDefaultFont("/usr/share/fonts/TTF/DroidSansMono.ttf");
    
    engine->designerGui->SetDefaultFont("/usr/share/fonts/TTF/DroidSansMono.ttf");
    engine->designerGui->SetSize(this->getSizeX(), this->getSizeY());
    
    engine->gui->LoadXml("gui.xml");
    engine->designerGui->LoadXml("designerGui.xml");
    
    engine->gui->ApplyAnchoring();
    engine->designerGui->ApplyAnchoring();
    
    
    float trackbarValue1 = 0;
    engine->designerGui->SubscribeEvent("1", ng::EVENT_TRACKBAR_CHANGE, [&](ng::Control *c) {  
        ng::TrackBar* p = (ng::TrackBar*)c;
        trackbarValue1 = p->GetValue() / 100.0;
    });
    
    float trackbarValue2 = 0;
    engine->designerGui->SubscribeEvent("2", ng::EVENT_TRACKBAR_CHANGE, [&](ng::Control *c) {
        ng::TrackBar* p = (ng::TrackBar*)c;
        trackbarValue2 = p->GetValue() / 10.0;
    });
    
    auto r = engine->gui->GetControlById("terminal")->GetRect();
    engine->gui->SubscribeEvent("terminal", ng::TextBox::event::enter, [&](ng::Control *c) {
		ng::TextBox* t = static_cast<ng::TextBox*>(c);
		try {
			Command::Execute(t->GetText());
		} catch(...) {}
		t->SetText("");
		engine->gui->Activate(0);
		t->SetVisible(false);
	});
    
    engine->camera.emplace_back(new he::Camera(glm::vec3(0.0f, 0.0f, 10.0f)));
    engine->camera.emplace_back(new he::Camera(engine->camera[0]->GetPosition()));

    engine->shader.emplace("frustum_bbox", new he::Shader("../Assets/Shaders/frustum.vs", "../Assets/Shaders/frustum.fs"));
    engine->shader.emplace("model_1", new he::Shader("../Assets/Shaders/test_03.vs", "../Assets/Shaders/test_03.fs"));
    engine->shader.emplace("model_1_visual", new he::Shader("../Assets/Shaders/test_03_visual.vs", "../Assets/Shaders/test_03_visual.fs", "../Assets/Shaders/test_03_visual.gs"));  
    
    he::ModelLoader loader;
    
    he::Model bob(engine->shader["model_1"]->GetShader());
    loader.LoadModel("../Assets/Models/guard/boblampclean.md5mesh", &bob);
    //bob.SetModelTrans(transformBob);
    
    he::Model pyro(engine->shader["model_1"]->GetShader());
    // loader.LoadModel("../Assets/Models/Pyro/Pyro.obj", &pyro);
    //test2.SetModelTrans(glm::translate(test2.modelTrans, glm::vec3(0.0, -2.0, -2.0)));
    
    
    he::Model sponza(engine->shader["model_1"]->GetShader());
    // loader.LoadModel("../Assets/Models/crytek-sponza/sponza.obj", &sponza);
    
    
    //add scale and rotate methods, and then after translation and/or rotation, scale by:
    //glm::vec3(0.07f, 0.07f, 0.07f)
    
    bool designerMode = false;
    bool toggleMouseRelative = false;
    bool toggleFullscreen = true;
    bool toggleWireframe = true;
    bool toggleCamera = true;
    
    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    
    if(toggleMouseRelative) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    float mouseScroll = 0.0;
    int skipMouseResolution = 0;
    
    bool running = true;
    
    Command::AddCommand("zoom", [&](float zoom) -> int {
		 if(toggleCamera) {
			engine->camera[0]->ProcessMouseScroll(zoom);
		} else {
			engine->camera[1]->ProcessMouseScroll(zoom);
		}
	});

    while(running)
    {
        this->loop();

        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) {
                running = false;
            }
            else if(e.type == SDL_KEYDOWN && !engine->gui->GetActiveControl()) {
                switch(e.key.keysym.sym)
                {
                    case SDLK_UP:
                    {
                        //camera[0]->ToggleLockY();
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
                    
                    case SDLK_RETURN: {
						ng::Control* terminal = engine->gui->GetControlById("terminal");
						terminal->SetVisible(true);
						engine->gui->Activate(terminal);
						continue;
						break;
                    }
                    
                    case SDLK_f:
                    {       
                        if(toggleFullscreen) {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            
                            //glViewport(0, 0, 1024, 768);
                            //GLint viewport[4];
                            //glGetIntegerv(GL_VIEWPORT, viewport);
                            
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            this->setSizeX(w);
                            this->setSizeY(h);
                            
                            // ng::Drawing::SetResolution(w,h);
                            engine->gui->SetSize(w,h);
                            engine->designerGui->SetSize(w,h);
                            
                            std::cout << this->getSizeX() << "x" << this->getSizeY() << std::endl;
                            
                            toggleFullscreen = false;
                        }
                        else {
                            SDL_SetWindowFullscreen(window, 0);
                            //SDL_SetWindowDisplayMode(window, 0);

                            int w, h;
                            SDL_GetWindowSize(window, &w, &h); //?
                            
                            this->setSizeX(800);
                            this->setSizeY(600);
                            
                            // ng::Drawing::SetResolution(800,600);
                            
                            engine->gui->SetSize(w,h);
                            engine->designerGui->SetSize(w,h);
                            
                            std::cout << this->getSizeX() << "x" << this->getSizeY() << std::endl;
                            
                            toggleFullscreen = true;
                        }
                        
                        skipMouseResolution = 2;     
                    }
                    break;
                    
                    case SDLK_SPACE:
                    {
                        /*
                        if(toggleMouseRelative) {
                            SDL_SetRelativeMouseMode(SDL_FALSE);
                            toggleMouseRelative = false;
                        }
                        else {
                            SDL_SetRelativeMouseMode(SDL_TRUE);
                            toggleMouseRelative = true;
                        }

                        skipMouseResolution = 2;
                        */
                        
                        if(designerMode) {
                            designerMode = false;
                        }
                        else {
                            designerMode = true;
                        }
                    }
                    break;
                    
                    case SDLK_e:
                    {
                        if(toggleWireframe) {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            toggleWireframe = false;
                        }
                        else {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                            toggleWireframe = true;
                        }
                    }
                    break;
                    
                    case SDLK_c:
                    {
                        if(toggleCamera) {
                            toggleCamera = false;
                        }
                        else {
                            toggleCamera = true;
                        }
                    }
                    break;
                    
                    case SDLK_p:
                        //this->takeScreenshot(0, 0, this->getSizeX(), this->getSizeY());
                        this->takeScreenshotPNG(0, 0, this->getSizeX(), this->getSizeY());
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
                    // mouseScroll = 0;
                    mouseScroll = e.wheel.y;
                    //mouseScroll += e.wheel.y;
                    break;

                default:
                    break;
                }
            }
            else if(e.type == SDL_MOUSEMOTION) {
                    //xpos = e.motion.x;
                    //ypos = e.motion.y;
                    //std::cout << "x: " << xpos << " y: " << ypos << std::endl;
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                toggleMouseRelative = true;

                skipMouseResolution = 2;
            }
            else if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT) {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                toggleMouseRelative = false;

                skipMouseResolution = 2;
            }
            
            engine->gui->OnEvent(e);
			engine->designerGui->OnEvent(e);
        }
        
        glViewport(0, 0, this->getSizeX(), this->getSizeY());
        
        

        // SDL_PumpEvents();
        const Uint8* state = SDL_GetKeyboardState(NULL);
        
        if(!engine->gui->GetActiveControl()) {
			if(state[SDL_SCANCODE_W]) {
				engine->camera[0]->ProcessKeyboard(he::Camera::MoveDirection::FORWARD, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_S]) {
				engine->camera[0]->ProcessKeyboard(he::Camera::MoveDirection::BACKWARD, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_A]) {
				engine->camera[0]->ProcessKeyboard(he::Camera::MoveDirection::LEFT, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_D]) {
				engine->camera[0]->ProcessKeyboard(he::Camera::MoveDirection::RIGHT, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_UP]) {
				engine->camera[1]->ProcessKeyboard(he::Camera::MoveDirection::FORWARD, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_DOWN]) {
				engine->camera[1]->ProcessKeyboard(he::Camera::MoveDirection::BACKWARD, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_LEFT]) {
				engine->camera[1]->ProcessKeyboard(he::Camera::MoveDirection::LEFT, this->getDeltaTime() * 2.0);
			}
			
			if(state[SDL_SCANCODE_RIGHT]) {
				engine->camera[1]->ProcessKeyboard(he::Camera::MoveDirection::RIGHT, this->getDeltaTime() * 2.0);
			}
		}
        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        int xpos;
        int ypos;
        SDL_GetRelativeMouseState(&xpos, &ypos);

        if(skipMouseResolution > 0 && (xpos != 0 || ypos != 0)) {
            skipMouseResolution--; 
        }
        else {
            if(toggleMouseRelative) {
                if(toggleCamera) {
                    engine->camera[0]->ProcessMouseMovement(xpos, ypos);
                    engine->camera[0]->ProcessMouseScroll(mouseScroll);
                } else {
                    engine->camera[1]->ProcessMouseMovement(xpos, ypos);
                    engine->camera[1]->ProcessMouseScroll(mouseScroll);
                }
                mouseScroll = 0;
                engine->gui->UnselectControl();
                engine->designerGui->UnselectControl();
            }
            else {
                int xpos;
                int ypos;
                SDL_GetMouseState(&xpos, &ypos);
                
                //std::cout << "x: " << xpos << " y: " << ypos << std::endl;
            }
        }
        
        glm::mat4 view = engine->camera[0]->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(engine->camera[0]->GetZoom()), this->getSizeX()/(float)this->getSizeY(), 0.1f, 1000.0f);

        glm::mat4 view2 = engine->camera[1]->GetViewMatrix();
        glm::mat4 projection2 = glm::perspective(glm::radians(engine->camera[1]->GetZoom()), this->getSizeX()/(float)this->getSizeY(), 0.1f, 100.0f);

        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0, -2.0, -2.0));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(trackbarValue1 * 360), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
        
        bob.SetTick(this->getTimeElapsed() * trackbarValue2);
        
        //works fine, but will bug due to AABB, what if camera is too close, looking at the model but doesnt see AABB vertices
        for(int i = 0; i < bob.GetBoundingBoxVertices().size(); ++i) {
            glm::vec3 transformedVector = glm::vec3(model * glm::vec4(bob.GetBoundingBoxVertices()[i], 1.0));
            
            if(engine->camera[0]->PointInFrustum(transformedVector)) {
                //std::cout << "vertex: " << i << " " << "in frustum! DRAW" << std::endl;
                
                bob.Draw(model, view, projection, engine->shader["model_1"]->GetShader());
                bob.Draw(model, view, projection, engine->shader["model_1_visual"]->GetShader());
                
                break;
            } else {
                //std::cout << "vertex: " << i << " " << "out of frustum! DONT DRAW" << std::endl;
            }
        }
        
        if(get_config_value("boundingbox"))
			bob.DrawBoundingBox(model, view, projection, engine->shader["frustum_bbox"]->GetShader());
        
        glm::mat4 model2;
        model2 = glm::translate(model2, glm::vec3(4.0f, -2.0f, -2.0f));
        model2 = glm::rotate(model2, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        
        pyro.DrawBoundingBox(model2, view, projection, engine->shader["frustum_bbox"]->GetShader());
        
        engine->camera[0]->ExtractFrustumPlanes(view, projection);
        engine->camera[1]->ExtractFrustumPlanes(view2, projection2);
        
        //visualize frustum
        if(get_config_value("frustum"))
			engine->camera[1]->DrawFrustum(model, view, projection, engine->shader["frustum_bbox"]->GetShader());
        
        glm::vec3 pyroAABBmin = glm::vec3(model2 * glm::vec4(pyro.GetBoundingBoxMin(), 1.0));
        glm::vec3 pyroAABBmax = glm::vec3(model2 * glm::vec4(pyro.GetBoundingBoxMax(), 1.0));
        
        if(engine->camera[1]->AABBIntersectsFrustum(pyroAABBmin, pyroAABBmax)) {
            glUseProgram(engine->shader["model_1"]->GetShader());
            // pyro.Draw(model2, view, projection);
            glUseProgram(0);
        }
        
        //draw points to test frustum intersection
        glm::mat4 identityModel2;
        identityModel2 = glm::translate(identityModel2, glm::vec3(0.0f, 0.0f, 0.0f));
        
        std::vector<glm::vec3> pointsPositions;
        std::vector<glm::vec3> pointsColors;
        for(int x = 0; x < 20; ++x) {
            for(int y = 0; y < 20; ++y) {
                for(int z = 0; z < 20; ++z) {
                    pointsPositions.push_back(glm::vec3(x, y, z));
                    pointsColors.push_back(glm::vec3(1.0, 0.0, 0.0));
                }
            }
        }
        
        for(int i = 0; i < pointsPositions.size(); ++i) {
            glm::vec3 transformedVector = glm::vec3(identityModel2 * glm::vec4(glm::vec3(pointsPositions[i]), 1.0));
            
            if(engine->camera[1]->PointInFrustum(transformedVector)) {
                pointsColors[i] = glm::vec3(0.0, 1.0, 0.0);
            } else {
                pointsColors[i] = glm::vec3(1.0, 0.0, 0.0);
            }
        }

		// draw frustum and points
		if(get_config_value("dots")) {
			glUseProgram(engine->shader["frustum_bbox"]->GetShader());
			glEnable(GL_PROGRAM_POINT_SIZE);
			
			GLuint vao_0, vbo_0, vbo_1;
			glGenVertexArrays(1, &vao_0);
			glBindVertexArray(vao_0);
			glGenBuffers(1, &vbo_0);
			glGenBuffers(1, &vbo_1);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo_0);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pointsPositions.size(), &pointsPositions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);    
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo_1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pointsColors.size(), &pointsColors[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(1);    
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			
			glUniformMatrix4fv(glGetUniformLocation(engine->shader["frustum_bbox"]->GetShader(), "model"), 1, GL_FALSE, glm::value_ptr(identityModel2));
			glUniformMatrix4fv(glGetUniformLocation(engine->shader["frustum_bbox"]->GetShader(), "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(engine->shader["frustum_bbox"]->GetShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));        

			glDrawArrays(GL_POINTS, 0, pointsPositions.size());
			glBindVertexArray(0);
			
			glDeleteVertexArrays(1, &vao_0);
			glDeleteBuffers(1, &vbo_0);
			glDeleteBuffers(1, &vbo_1);
			
			glDisable(GL_PROGRAM_POINT_SIZE);
			glUseProgram(0);
        }
        
        glUseProgram(engine->shader["model_1"]->GetShader());
        //sponza.Draw(model2, view, projection);
        glUseProgram(0);
        
        
        engine->gui->Render();
        
        if(designerMode) {
            engine->designerGui->Render();
        }
        
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

void App::takeScreenshotPNG(int x, int y, int width, int height)
{

}


