#include "Shader.hpp"

namespace Helix {
    Shader::Shader() {}
    
    Shader::Shader(std::string shaderName, std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile)
    {
        this->loadShader(shaderName, vertexShaderFile, fragmentShaderFile, geometryShaderFile);
    }
    
    Shader::~Shader() {}

    void Shader::loadShader(std::string shaderName, std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile)
    {
        GLuint vertexShader = this->readShader(vertexShaderFile, GL_VERTEX_SHADER);
        GLuint geometryShader;
        if(geometryShaderFile.length() > 0) {
            geometryShader = this->readShader(geometryShaderFile, GL_GEOMETRY_SHADER);
        }
        GLuint fragmentShader = this->readShader(fragmentShaderFile, GL_FRAGMENT_SHADER);
        
        // linking
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        if(geometryShaderFile.length() > 0) {
            glAttachShader(shaderProgram, geometryShader);
        }
        glAttachShader(shaderProgram, fragmentShader);
        
        glLinkProgram(shaderProgram); 
        
        if(!shaderProgram) {
            throw std::string("Failed to create shader") + shaderName;
        }

        // shader was compiled and linked, now add GLuint shaderProgram to std::map ===> should be moved to Game
        m_loadedShaders.insert(std::pair<std::string,GLuint>(shaderName, shaderProgram));
        
        /*
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        */
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
        if (m_loadedShaders.find(shaderName) == m_loadedShaders.end()) {        
            throw std::string("Shader could not be used, missing in std::map");
        }
        
        glUseProgram(m_loadedShaders[shaderName]);
    }
    
    GLuint Shader::GetShader(std::string shaderName)
    {
        if (m_loadedShaders.find(shaderName) == m_loadedShaders.end()) {
            throw std::string("Shader could not be used, missing in std::map");
        }
        
        return m_loadedShaders[shaderName];
    }
    
    void Shader::InitPyro(std::string shaderName, float time, glm::mat4 view, glm::mat4 projection, float posX, float posY, float posZ)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        //glEnable(GL_MULTISAMPLE);
        
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        //glUseProgram(m_loadedShaders[shaderName]);
                
        glm::vec3 lightPos(2.7f, 0.2f, 2.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
 
        float moveX = sin(time) * 0.8f;
        float moveY = cos(time) * 0.8f;
        
        GLint lightPosLoc = glGetUniformLocation(m_loadedShaders[shaderName], "lightPos");
        GLint viewPosLoc = glGetUniformLocation(m_loadedShaders[shaderName], "viewPos");
        GLint lightColorLoc = glGetUniformLocation(m_loadedShaders[shaderName], "lightColor");
        
        glUniform3f(lightPosLoc, lightPos.x * moveX, lightPos.y, lightPos.z * moveY);
        glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);   
        
        //glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
        //glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

        //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        //glm::mat4 projection = glm::perspective(45.0f, 800.0f/600.0f, 0.1f, 100.0f);
        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(posX, posY, posZ));
        model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 mvp = projection * view * model;

        GLint modelLoc = glGetUniformLocation(m_loadedShaders[shaderName], "model");
        GLint mvpLoc = glGetUniformLocation(m_loadedShaders[shaderName], "mvp");
        //GLint viewLoc = glGetUniformLocation(m_loadedShaders[shaderName], "view");
        //GLint projLoc = glGetUniformLocation(m_loadedShaders[shaderName], "projection");
        
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        
        //here should go draw
        
        //glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        //glUseProgram(0);
    }
    
    void Shader::InitBob(std::string shaderName, float time, glm::mat4 view, glm::mat4 projection, float posX, float posY, float posZ)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        //glEnable(GL_MULTISAMPLE);
        
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        //glUseProgram(m_loadedShaders[shaderName]);
                
        glm::vec3 lightPos(2.7f, 0.2f, 2.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
 
        float moveX = sin(time) * 0.8f;
        float moveY = cos(time) * 0.8f;
        
        GLint lightPosLoc = glGetUniformLocation(m_loadedShaders[shaderName], "lightPos");
        GLint viewPosLoc = glGetUniformLocation(m_loadedShaders[shaderName], "viewPos");
        GLint lightColorLoc = glGetUniformLocation(m_loadedShaders[shaderName], "lightColor");
        
        glUniform3f(lightPosLoc, lightPos.x * moveX, lightPos.y, lightPos.z * moveY);
        glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);   
        
        //glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
        //glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

        //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        //glm::mat4 projection = glm::perspective(45.0f, 800.0f/600.0f, 0.1f, 100.0f);
        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(posX, posY, posZ));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));  
        
        glm::mat4 mvp = projection * view * model;

        GLint modelLoc = glGetUniformLocation(m_loadedShaders[shaderName], "model");
        GLint mvpLoc = glGetUniformLocation(m_loadedShaders[shaderName], "mvp");
        //GLint viewLoc = glGetUniformLocation(m_loadedShaders[shaderName], "view");
        //GLint projLoc = glGetUniformLocation(m_loadedShaders[shaderName], "projection");
        
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        
        //here should go draw
        
        //glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        //glUseProgram(0);
    }
}

