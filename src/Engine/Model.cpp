#include "Model.hpp"

namespace Helix {
    Model::Model() {}
    
    Model::Model(std::string fileName)
    {
        this->loadModel(fileName);
    }
    
    Model::~Model() {}
    
    void Model::loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_ImproveCacheLocality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::string("Assimp error: ") + importer.GetErrorString();
        }

        m_directory = path.substr(0, path.find_last_of('/'));
        
        m_GlobalInverseTransform = scene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        
        this->processNode(scene->mRootNode, scene);
    }
    
    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        for(GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
            m_meshes.push_back(this->processMesh(mesh, scene));         
        }

        for(GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }
    }
    
    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Mesh::Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Mesh::Texture> textures;
        
        if(!mesh->HasNormals()) {
            throw std::string("Loading model failed! Missing normals. Try adding flag: aiProcess_GenNormals or aiProcess_GenSmoothNormals");
        }

        for(GLuint i = 0; i < mesh->mNumVertices; i++) {
            Mesh::Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // Does the mesh contain texture coordinates?
            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            
            vertices.push_back(vertex);
        }

        for(GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];

            for(GLuint j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Mesh::Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Mesh::Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return Mesh(vertices, indices, textures);
    }
    
    std::vector<Mesh::Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Mesh::Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            
            for(GLuint j = 0; j < m_textures_loaded.size(); j++) {
                if(m_textures_loaded[j].path == str) {
                    textures.push_back(m_textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            
            // load texture, if was not loaded yet
            if(!skip) {
                Mesh::Texture texture;
                texture.id = textureFromFile(str.C_Str());
                texture.type = typeName;
                texture.path = str;
                
                textures.push_back(texture);
                m_textures_loaded.push_back(texture);
            }
        }
        
        return textures;
    }
    
    GLint Model::textureFromFile(std::string path)
    {
        std::string filename = m_directory + '/' + path;
 
        SDL_Surface* image = IMG_Load(filename.c_str());
        if(!image) {
            throw std::string("Error loading image: ") + IMG_GetError();
        }
        
        GLint colorMode;
        if(image->format->BytesPerPixel == 4) {
            if(image->format->Rmask == 0x000000ff) {
                colorMode = GL_RGBA;
            }
            else {
                colorMode = GL_BGRA;
            }
        }
        else if(image->format->BytesPerPixel == 3) {
            if(image->format->Rmask == 0x000000ff) {
                colorMode = GL_RGB;
            }
            else {
                colorMode = GL_BGR;
            }
        }
        else {
             throw std::string("Image is not truecolor!");
        }
        
        GLuint textureID;
        glGenTextures(1, &textureID);
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, colorMode, GL_UNSIGNED_BYTE, image->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);   
        
         
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        SDL_FreeSurface(image);
        
        return textureID;
    }
    
    void Model::Draw(GLuint shader)
    {
        for(GLuint i = 0; i < m_meshes.size(); i++) {
            m_meshes[i].Draw(shader);
        }
    }
}

