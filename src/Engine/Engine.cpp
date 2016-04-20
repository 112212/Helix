#include "Engine.hpp"

namespace Helix {
    Engine* Engine::m_instance = nullptr;
    
    Engine::Engine() {};
    Engine::~Engine() {};
    
    Engine* Engine::Instance()
    {
        //static Engine instance;

        if(m_instance == nullptr) {
            m_instance = new Engine();
        }
        
        return m_instance;
    }
    
    void Engine::Init()
    {
        camera.emplace_back(new he::Camera(glm::vec3(0.0f, 0.0f, 0.0f)));
        camera.emplace_back(new he::Camera(this->camera[0]->GetPosition()));
        
        shader.emplace("frustumShader", new he::Shader("../Assets/Shaders/frustum.vs", "../Assets/Shaders/frustum.fs"));
    }
}
