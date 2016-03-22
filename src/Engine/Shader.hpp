#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>

//remove with assimp model load?
#include <vector>
#include <SDL2/SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Helix {
    class Shader {
        public:
            Shader();
            ~Shader();

            void LoadShader(std::string vertexShaderFile, std::string fragmentShaderFile, std::string shaderName);
            void UseShader(std::string shaderName);
            
            //should be removed (move code to game)
            void Init(SDL_Surface* surface, std::string shaderName);
            void Draw(float time);
            
         private:
            GLuint readShader(std::string shaderFile, GLenum shaderType);
            std::map<std::string,GLuint> m_loadedShaders;
            
            //should be removed (move code to game)
            glm::mat4 projection;
            glm::mat4 view;
            GLint uniform_m_mvp;
            
            std::vector<GLfloat> verticesVector;
            std::vector<GLushort> indicesVector;
            std::vector<GLfloat> cube_texcoordsVector;
    };  
}
