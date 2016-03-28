#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Helix {
    class Shader {
        public:
            Shader();
            Shader(std::string shaderName, std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile = "");
            ~Shader();

            void loadShader(std::string shaderName, std::string vertexShaderFile, std::string fragmentShaderFile, std::string geometryShaderFile = "");
            void UseShader(std::string shaderName);
            GLuint GetShader(std::string shaderName);
            
            //should be moved to game code & merged with model->Draw
            void InitPyro(std::string shaderName, float time, glm::vec3 cameraPos, glm::mat4 view, glm::mat4 projection, float posX = 0, float posY = -2, float posZ = -2);
            void InitBob(std::string shaderName, float time, glm::vec3 cameraPos, glm::mat4 view, glm::mat4 projection, float posX = 0, float posY = -2, float posZ = -2);

         private:
            GLuint readShader(std::string shaderFile, GLenum shaderType);
            //move m_loadedShaders outside?
            std::map<std::string,GLuint> m_loadedShaders;
            
            //should be removed (move code to game)
            glm::mat4 projection;
            glm::mat4 view;
            GLint uniform_m_mvp;
    };  
}
