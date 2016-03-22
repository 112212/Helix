#include "Shader.hpp"

namespace Helix {
    Shader::Shader() {};
    Shader::~Shader() {};

    void Shader::LoadShader(std::string vertexShaderFile, std::string fragmentShaderFile, std::string shaderName)
    {
        GLuint vertexShader = this->readShader(vertexShaderFile, GL_VERTEX_SHADER);
        GLuint fragmentShader = this->readShader(fragmentShaderFile, GL_FRAGMENT_SHADER);
        
        // linking
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        
        glLinkProgram(shaderProgram); 
        
        if(!shaderProgram) {
            throw std::string("Failed to create shader") + shaderName;
        }
        
        // shader was compiled and linked, now add GLuint shaderProgram to std::map ===> should be moved to Game
        m_loadedShaders.insert(std::pair<std::string,GLuint>(shaderName, shaderProgram));
    }
    
    GLuint Shader::readShader(std::string shaderFile, GLenum shaderType)
    {
        // reading shader
        std::ifstream shaderStream(shaderFile);       
        if(!shaderStream) {
            throw std::string("Failed to load shader file: ") + shaderFile;
        }
        
        std::stringstream shaderData;
        
        shaderData << shaderStream.rdbuf(); 
        shaderStream.close();
        
        const std::string &shaderString = shaderData.str();
        const char *shaderSource = shaderString.c_str();
        GLint shaderLength = shaderString.size();
        
        // creating shader
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, (const GLchar**)&shaderSource, (GLint*)&shaderLength);
        
        // compiling shader
        GLint compileStatus;
        
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        
        if(compileStatus != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(shader, 512, NULL, buffer);
            
            throw std::string(buffer);
        }
        
        return shader;
    }
    
    void Shader::UseShader(std::string shaderName)
    {
        //remove find due to slow lookup?
        if (m_loadedShaders.find(shaderName) != m_loadedShaders.end()) {
            glUseProgram(m_loadedShaders[shaderName]);
        } else {
            throw std::string("Shader could not be used, missing in std::map");
        }
    }
    
    void Shader::Init(SDL_Surface* surface, std::string shaderName)
    {
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
        
        for(std::size_t i = 0; i < sizeof(vertices); i++) {
            verticesVector.push_back(vertices[i]);
        }
        
        for(std::size_t i = 0; i < sizeof(indices); i++) {
            indicesVector.push_back(indices[i]);
        }
        
        for(std::size_t i = 0; i < sizeof(cube_texcoords); i++) {
            cube_texcoordsVector.push_back(cube_texcoords[i]);
        }
        
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
       
        GLint position = glGetAttribLocation(m_loadedShaders[shaderName], "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        GLint attribute_v_color = glGetAttribLocation(m_loadedShaders[shaderName], "v_color");
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
            
        GLint attribute_texcoord = glGetAttribLocation(m_loadedShaders[shaderName], "texcoord");
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
        glEnableVertexAttribArray(attribute_texcoord);
        glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
     
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uniform_mytexture, GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        uniform_m_mvp = glGetUniformLocation(m_loadedShaders[shaderName], "mvp");
        
        projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 10.0f);
        view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));

        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_MULTISAMPLE); 
        
    }
    
    void Shader::Draw(float time)
    {
        this->UseShader("testShader1");
        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        float angle = time * 20;
        float move = 0;
        
        glm::vec3 axis_z(0.0, 1.0, 0.0);
        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, -4.0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        
        glm::mat4 mvp = projection * view * model;
        
        glUniformMatrix4fv(uniform_m_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        
        glDrawElements(GL_TRIANGLES, sizeof(indicesVector), GL_UNSIGNED_SHORT, 0);
    }
}

