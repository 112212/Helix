#include "Camera.hpp"

namespace Helix {
    Camera::Camera() {}
    
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    {
        m_movement_speed = SPEED;
        m_mouse_sensitivity = SENSITIVITY;
        m_zoom = ZOOM;
        
        m_front = glm::vec3(0.0f, 0.0f, -1.0f);
        
        m_position = position;
        m_world_up = up;
        m_yaw = yaw;
        m_pitch = pitch;
        
        this->updateCameraVectors();
    }
    
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    {
        m_movement_speed = SPEED;
        m_mouse_sensitivity = SENSITIVITY;
        m_zoom = ZOOM;
        
        m_front = glm::vec3(0.0f, 0.0f, -1.0f);
        
        m_position = glm::vec3(posX, posY, posZ);
        m_world_up = glm::vec3(upX, upY, upZ);
        m_yaw = yaw;
        m_pitch = pitch;
        
        this->updateCameraVectors();
    }
    
    Camera::~Camera() {}
    
    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
    
    void Camera::ProcessKeyboard(MoveDirection direction, float dt)
    {
        float velocity = m_movement_speed * dt;
        
        if (direction == FORWARD) {
            m_position += m_front * velocity;
        }
        
        if (direction == BACKWARD) {
            m_position -= m_front * velocity;
        }
        
        if (direction == LEFT) {
            m_position -= m_right * velocity;
        }
        
        if (direction == RIGHT) {
            m_position += m_right * velocity;
        }
    }
    
    void Camera::ProcessMouseMovement(float offsetX, float offsetY, bool constrainPitch)
    {
        offsetX *= m_mouse_sensitivity;
        offsetY *= m_mouse_sensitivity;

        m_yaw += offsetX;
        m_pitch += -offsetY;

        if (constrainPitch) {
            if (m_pitch > 89.0f) {
                m_pitch = 89.0f;
            }
            
            if (m_pitch < -89.0f) {
                m_pitch = -89.0f;
            }
        }

        this->updateCameraVectors();
    }
    
    void Camera::ProcessMouseScroll(float offsetY)
    {
        if (m_zoom >= 1.0f && m_zoom <= 45.0f) {
            m_zoom -= offsetY;
        }
        
        if (m_zoom <= 1.0f) {
            m_zoom = 1.0f;
        }
        
        if (m_zoom >= 45.0f) {
            m_zoom = 45.0f;
        }
    }
    
    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        
        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, m_world_up));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
    
    float Camera::GetZoom()
    {
        return m_zoom;
    }
    
    glm::vec3 Camera::GetPosition()
    {
        return m_position;
    }
}

