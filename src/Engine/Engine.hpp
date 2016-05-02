//opengl
#include <GL/glew.h>

//sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

//stl
#include <unordered_map>

//helix
#include "Shader.hpp"
#include "Model.hpp"
#include "Camera.hpp"

//gui
#include "Gui.hpp"
#include "XmlLoader.hpp"
#include "Control.hpp"
#include "controls/TrackBar.hpp"
#include "common/SDL/Drawing.hpp"

namespace he = Helix;

namespace Helix {
    class Engine {
        public:
            static Engine* Instance();
            void Init();
            
            std::vector<he::Camera*> camera;
            std::unordered_map<std::string, he::Shader*> shader;
            
            ng::GuiEngine* gui;
            
        private:
            Engine();
            ~Engine();
            
            static Engine* m_instance;
    };
}
