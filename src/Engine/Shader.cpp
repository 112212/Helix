#include "Shader.hpp"

namespace Helix {
    Shader::Shader() {}
    
    Shader::Shader(std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile)
    {
        this->loadShader(vertexShaderFile, fragmentShaderFile, geometryShaderFile);
    }
    
    Shader::~Shader()
    {
        glDeleteProgram(m_shaderProgram);
    }

    void Shader::loadShader(std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile)
    {
        GLuint vertexShader = this->readShader(vertexShaderFile, GL_VERTEX_SHADER);
        GLuint geometryShader = 0;
        if(geometryShaderFile.length() > 0) {
            geometryShader = this->readShader(geometryShaderFile, GL_GEOMETRY_SHADER);
        }
        GLuint fragmentShader = this->readShader(fragmentShaderFile, GL_FRAGMENT_SHADER);
        
        // linking
        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        if(geometryShaderFile.length() > 0) {
            glAttachShader(m_shaderProgram, geometryShader);
        }
        glAttachShader(m_shaderProgram, fragmentShader);
        
        glLinkProgram(m_shaderProgram); 
        
        if(!m_shaderProgram) {
            throw std::string("Failed to create shader program");
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
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
    
    void Shader::UseShader()
    {
        glUseProgram(m_shaderProgram);
    }
    
    void Shader::UnuseShader()
    {
        glUseProgram(0);
    }
    
    GLuint Shader::GetShader()
    {        
        return m_shaderProgram;
    }
}

