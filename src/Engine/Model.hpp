#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

//remove sdl and move sdl surface image to shaders?
#include <SDL2/SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"

namespace Helix {
    class Model {
        public:
            Model();
            Model(std::string fileName);
            ~Model();
            
            void Draw(GLuint shader);
            
        private:
            void loadModel(std::string path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
            GLint textureFromFile(std::string path);
        
            std::vector<Mesh::Texture> m_textures_loaded; 
            std::vector<Mesh> m_meshes;
            
            aiMatrix4x4t<float> m_GlobalInverseTransform;
            
            std::string m_directory;
    };
}
