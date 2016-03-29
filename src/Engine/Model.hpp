#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

//remove sdl and move sdl surface image to shaders class? Use SOIL instead?
#include <SDL2/SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MAX_BONES 64

namespace Helix {
    // reorganize private and public
    struct Model
    {
        Model();
        Model(GLuint shader);
        ~Model();
        
        void SetModelTrans(glm::mat4);
        
        struct Mesh
        {
            std::vector<glm::vec3> vertices;
            std::vector<unsigned int> indices;
            std::vector<glm::vec2> uvs;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec4> weights;
            std::vector<glm::vec4> boneID;
 
            glm::mat4 baseModelMatrix;
 
            GLuint vao, vbo, nbo, ebo, uvb, tex, wbo, idbo;
            GLint posAttribute, normalAttribute, texAttribute, weightAttribute, boneAttribute;
            GLuint modelID, viewID, projectionID, transID, modelTransID;
            
            SDL_Surface* image;
            
            bool hasAnimation;
        };
 
        struct Animation
        {
            std::string name;
            double duration;
            double ticksPerSecond;
            // all of the bone transformations, this is modified every frame
            // assimp calls it a channel, its anims for a node aka bone
            std::vector<glm::mat4> boneTrans;
            std::map<std::string, glm::mat4> boneOffset;
 
            struct Channel
            {
                std::string name;
                glm::mat4 offset;
                std::vector<aiVectorKey> mPositionKeys;
                std::vector<aiQuatKey> mRotationKeys;
                std::vector<aiVectorKey> mScalingKeys;
            };
            std::vector<Channel> channels;
 
            struct BoneNode
            {
                std::string name;
                BoneNode* parent;
                std::vector<BoneNode> children;
                glm::mat4 nodeTransform;
                glm::mat4 boneTransform;
            };
            BoneNode root;
 
            void buildBoneTree(const aiScene* scene, aiNode* node, BoneNode* bNode, Model* m);
 
            //aiAnimation* data;
        };
        // all of the animations
        std::vector<Animation> animations;
        unsigned int currentAnim;
        //void setAnimation(std::string name);
        std::vector<std::string> animNames;
        // map of bones
        std::map<std::string, unsigned int> boneID;
        // runs every frame
        void tick(double time);
        void updateBoneTree(double time, Animation::BoneNode* node, glm::mat4 transform);
            
        glm::mat4 modelTrans;
        //GLuint modelTransID; moved to mesh
       
        std::string rootPath;
 
        //todo: make init and render optional
        std::vector<Mesh> meshes;
        GLuint shader;
        bool modelLoaded;
        
        void init();
        void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    };
 
    class ModelLoader
    {
        public:
            ModelLoader();
            ~ModelLoader();
            
            // this will load all of the required data and dump it into the model struct
            void LoadModel(std::string fileName, Model* m);
            
        private:
            void processNode(const aiScene* scene, aiNode* node, Model* m);
            void processMesh(const aiScene* scene, aiNode* node, aiMesh* mesh, Model* m);
            void processAnimations(const aiScene* scene, Model* m);
            
            SDL_Surface* textureSurface;
    };
 
    glm::mat4 toMat4(aiMatrix4x4* ai);
}
