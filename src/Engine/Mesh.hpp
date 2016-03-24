#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Helix {
    class Mesh {
        public:
            struct Vertex {
                glm::vec3 Position;
                glm::vec3 Normal;
                glm::vec2 TexCoords;
            };
            
            struct Texture {
                GLuint id;
                std::string type;
                aiString path;
            };
    
            Mesh();
            Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
            ~Mesh();
            
            void Draw(GLuint shader);
            
            std::vector<Vertex> m_vertices;
            std::vector<GLuint> m_indices;
            std::vector<Texture> m_textures;
            
        private:
            void setupMesh();
            
            GLuint m_VAO, m_VBO, m_EBO;
    };  
}
