#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

namespace Helix {
    class Shader {
        public:
            Shader();
            ~Shader();
            
            GLuint LoadShader(const char* vertexShaderFile, const char* fragmentShaderFile);
    };  
}
