#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Gui.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Camera.hpp"

namespace he = Helix;

namespace Helix {
    class Engine {
        public:
            static Engine* Instance();
            void Init();
            
            std::vector<he::Camera*> camera;
            
        private:
            Engine();
            ~Engine();
            
            static Engine* m_instance;
    };
}
