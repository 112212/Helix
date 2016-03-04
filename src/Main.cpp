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
    
    he::Fps fps;
    fps.Init();
    
    he::Shader shader;
    GLuint shaderProgram = shader.LoadShader("../src/Engine/Shaders/test_01.vs", "../src/Engine/Shaders/test_01.fs");
    
    SDL_Surface* surface = IMG_Load("../src/Engine/Shaders/texture.jpg");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat vertices[] = {
        // front
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // top
        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
        // back
         1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
        // bottom
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        // left
        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        // right
         1.0, -1.0,  1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0,  1.0,  1.0, 
    };
    
    GLushort indices[] = {
        // front
         0,  1,  2,
         2,  3,  0,
        // top
         4,  5,  6,
         6,  7,  4,
        // back
         8,  9, 10,
        10, 11,  8,
        // bottom
        12, 13, 14,
        14, 15, 12,
        // left
        16, 17, 18,
        18, 19, 16,
        // right
        20, 21, 22,
        22, 23, 20,
    };
    
    GLfloat cube_texcoords[] = {
        // front
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        //top
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        //back
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        //bottom
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        //left
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        //right
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
    };
    
    GLuint vbo_cube_texcoords;
    glGenBuffers(1, &vbo_cube_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
   
    GLint position = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    GLint attribute_v_color = glGetAttribLocation(shaderProgram, "v_color");
    glEnableVertexAttribArray(attribute_v_color);
    glVertexAttribPointer(attribute_v_color, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
    
    GLuint texture_id, program_id;
    GLint uniform_mytexture;
    
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->w,
        surface->h,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels);
        
    GLint attribute_texcoord = glGetAttribLocation(shaderProgram, "texcoord");
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
    glEnableVertexAttribArray(attribute_texcoord);
    glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uniform_mytexture, GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLint uniform_m_mvp = glGetUniformLocation(shaderProgram, "mvp");
    
    glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE); 
    
    float angle = 0;

    auto t_start = std::chrono::high_resolution_clock::now();
    
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

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        //glUniform3f(outColor, 0.0f, (sin(time * 8.0f) + 1.0f) / 2.0f, 0.0f);
        
        //float move = (sin(time * 1.0f) + 1.0f) / 2.0f;
        float move = 0;
        
        //float angle = time * 45;
        //float angle = 0;
        
        float angl = time * 0.1;
          glm::mat4 anim = 
            glm::rotate(glm::mat4(1.0f), angl*2.0f, glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), angl*2.0f, glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), angl*2.0f, glm::vec3(0, 0, 1));
        
        glm::vec3 axis_z(0.0, 1.0, 0.0);
        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, -4.0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        
        glm::mat4 mvp = projection * view * model;
        
        glUniformMatrix4fv(uniform_m_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, 0);
        
        //SDL_GL_SwapWindow(window);
        
        SDL_RenderPresent(renderer);

        fps.Show();
        
        //SDL_Delay(16);
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
