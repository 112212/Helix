#include "Camera.hpp"

namespace Helix {
    Camera::Camera() {}
    
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    {
        m_movement_speed = SPEED;
        m_mouse_sensitivity = SENSITIVITY;
        m_zoom = ZOOM;
        
        m_position = position;
        m_world_up = up;
        m_yaw = yaw;
        m_pitch = pitch;
        
        m_lock_y = false;
        
        this->updateCameraVectors();
    }
    
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    {
        m_movement_speed = SPEED;
        m_mouse_sensitivity = SENSITIVITY;
        m_zoom = ZOOM;

        m_position = glm::vec3(posX, posY, posZ);
        m_world_up = glm::vec3(upX, upY, upZ);
        m_yaw = yaw;
        m_pitch = pitch;
        
        this->updateCameraVectors();
    }
    
    Camera::~Camera() {}
    
    void Camera::ProcessKeyboard(MoveDirection direction, float dt)
    {
        glm::vec3 lastCameraPosition = m_position;
        
        float velocity = m_movement_speed * dt;
        
        if(direction == FORWARD) {
            m_position += m_front * velocity;
        }
        
        if(direction == BACKWARD) {
            m_position -= m_front * velocity;
        }
        
        if(direction == LEFT) {
            m_position -= m_right * velocity;
        }
        
        if(direction == RIGHT) {
            m_position += m_right * velocity;
        }
        
        // disable flying, true fps camera
        if(m_lock_y) {
            m_position.y = lastCameraPosition.y;
            //m_position.y = 0.0f;
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
    
    void Camera::SetPosition(glm::vec3 position)
    {
        m_position = position;
    }
    
    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
    
    float Camera::GetZoom() const
    {
        return m_zoom;
    }
    
    glm::vec3 Camera::GetPosition() const
    {
        return m_position;
    }
    
    void Camera::ToggleLockY()
    {
        if(m_lock_y) {
            m_lock_y = false;
        }
        else {
            m_lock_y = true;
        }
    }
    
    void Camera::ExtractFrustumPlanes(glm::mat4 view, glm::mat4 projection)
    {
        glm::mat4 matrix = projection * view;

        m_frustum_planes[0] = glm::row(matrix, 3) + glm::row(matrix, 0); //left
        m_frustum_planes[1] = glm::row(matrix, 3) - glm::row(matrix, 0); //right
        m_frustum_planes[2] = glm::row(matrix, 3) + glm::row(matrix, 1); //bottom
        m_frustum_planes[3] = glm::row(matrix, 3) - glm::row(matrix, 1); //top
        m_frustum_planes[4] = glm::row(matrix, 3) + glm::row(matrix, 2); //near
        m_frustum_planes[5] = glm::row(matrix, 3) - glm::row(matrix, 2); //far
        
        for(int i = 0; i < 6; ++i) {
            m_frustum_planes[i] /= -glm::length(glm::vec3(m_frustum_planes[i]));
            
            /*
            std::cout << "FRUSTUM: A: " << m_frustum_planes[i].x << " B: " << m_frustum_planes[i].y << " C: " << 
            m_frustum_planes[i].z << " D: " << m_frustum_planes[i].w << std::endl;
            */
        }
        
        m_frustum_vertices[0] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[0], m_frustum_planes[2], m_frustum_planes[4]));
        m_frustum_vertices[1] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[0], m_frustum_planes[3], m_frustum_planes[4]));
        m_frustum_vertices[2] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[1], m_frustum_planes[3], m_frustum_planes[4]));
        m_frustum_vertices[3] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[1], m_frustum_planes[2], m_frustum_planes[4]));

        m_frustum_vertices[4] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[0], m_frustum_planes[2], m_frustum_planes[5]));
        m_frustum_vertices[5] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[0], m_frustum_planes[3], m_frustum_planes[5]));
        m_frustum_vertices[6] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[1], m_frustum_planes[3], m_frustum_planes[5]));
        m_frustum_vertices[7] = glm::vec3(threePlanesIntersectionPoint(m_frustum_planes[1], m_frustum_planes[2], m_frustum_planes[5]));
    }
    
    void Camera::PrintFrustumVerticesPositions()
    {
        std::cout << "NEAR PLANE" << std::endl;

        std::cout << "intersection of left, bottom, near is at x: " << m_frustum_vertices[0].x << " y: " << m_frustum_vertices[0].y << " z: " << m_frustum_vertices[0].z << std::endl;
        std::cout << "intersection of left, top, near is at x: " << m_frustum_vertices[1].x << " y: " << m_frustum_vertices[1].y << " z: " << m_frustum_vertices[1].z << std::endl;
        std::cout << "intersection of right, top, near is at x: " << m_frustum_vertices[2].x << " y: " << m_frustum_vertices[2].y << " z: " << m_frustum_vertices[2].z << std::endl;
        std::cout << "intersection of right, bottom, near is at x: " << m_frustum_vertices[3].x << " y: " << m_frustum_vertices[3].y << " z: " << m_frustum_vertices[3].z << std::endl;

        std::cout << "FAR PLANE" << std::endl;
        std::cout << "intersection of left, bottom, far is at x: " << m_frustum_vertices[4].x << " y: " << m_frustum_vertices[4].y << " z: " << m_frustum_vertices[4].z << std::endl;
        std::cout << "intersection of left, top, far is at x: " << m_frustum_vertices[5].x << " y: " << m_frustum_vertices[5].y << " z: " << m_frustum_vertices[5].z << std::endl;
        std::cout << "intersection of right, top, far is at x: " << m_frustum_vertices[6].x << " y: " << m_frustum_vertices[6].y << " z: " << m_frustum_vertices[6].z << std::endl;
        std::cout << "intersection of right, bottom, far is at x: " << m_frustum_vertices[7].x << " y: " << m_frustum_vertices[7].y << " z: " << m_frustum_vertices[7].z << std::endl;
    }
    
    bool Camera::PointInFrustum(glm::vec3 center)
    {
        for (const auto& plane : m_frustum_planes) {
            float dist = plane.x * center.x + plane.y * center.y + plane.z * center.z + plane.w;
            if (dist > 0) {
                return false;
            }
        }
        return true;
    }
    
    bool Camera::SphereInFrustum(glm::vec3 center, float radius)
    {
        for (const auto& plane : m_frustum_planes) {
            float dist = plane.x * center.x + plane.y * center.y + plane.z * center.z + plane.w - radius;
            if (dist > 0) {
                return false;
            }
        }
        return true;
    }
    
    glm::vec3 Camera::threePlanesIntersectionPoint(glm::vec4 a, glm::vec4 b, glm::vec4 c)
    {
        // Formula used
        //                d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
        //P =   ---------------------------------------------------------------------
        //                             N1 . ( N2 * N3 )
        //
        // Note: N refers to the normal, d refers to the displacement. '.' means dot product. '*' means cross product

        glm::vec3 v1, v2, v3;
        float denominator = -glm::dot(glm::vec3(a), glm::cross(glm::vec3(b), glm::vec3(c)));

        v1 = (a.w * (glm::cross(glm::vec3(b), glm::vec3(c))));
        v2 = (b.w * (glm::cross(glm::vec3(c), glm::vec3(a))));
        v3 = (c.w * (glm::cross(glm::vec3(a), glm::vec3(b))));

        glm::vec3 numerator = glm::vec3(v1.x + v2.x + v3.x, v1.y + v2.y + v3.y, v1.z + v2.z + v3.z);
        
        return numerator / denominator;
    }
    
}

