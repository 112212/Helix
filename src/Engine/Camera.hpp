#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Helix {
    class Camera {
        public:
            static constexpr float YAW = -90.0f;
            static constexpr float PITCH = 0.0f;
            static constexpr float SPEED = 3.0f;
            static constexpr float SENSITIVITY = 0.25f;
            static constexpr float ZOOM = 45.0f;
            
            enum MoveDirection {
                FORWARD,
                BACKWARD,
                LEFT,
                RIGHT
            };
    
            Camera();
            Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
            Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
            ~Camera();

            void ProcessKeyboard(MoveDirection direction, float dt);
            void ProcessMouseMovement(float offsetX, float offsetY, bool constrainPitch = true);
            void ProcessMouseScroll(float offsetY);   
            float GetZoom() const;
            glm::mat4 GetViewMatrix() const;
            glm::vec3 GetPosition() const;
            void ToggleLockY();
            
        private:
            void updateCameraVectors();
            
            glm::vec3 m_position;
            glm::vec3 m_front;
            glm::vec3 m_up;
            glm::vec3 m_right;
            glm::vec3 m_world_up;
            
            float m_yaw;
            float m_pitch;
            float m_movement_speed;
            float m_mouse_sensitivity;
            float m_zoom;
            
            bool m_lock_y;
    };  
}
