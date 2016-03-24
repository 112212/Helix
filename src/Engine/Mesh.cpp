#include "Mesh.hpp"

namespace Helix {
    Mesh::Mesh() {}
    
    Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;

        this->setupMesh();
    }
    
    Mesh::~Mesh() {}
    
    void Mesh::Draw(GLuint shader) 
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        for(GLuint i = 0; i < m_textures.size(); i++)
        {
            
            // Retrieve texture number (the N in texture_diffuseN)
            std::stringstream ss;
            std::string number;
            std::string name = m_textures[i].type;
            if(name == "texture_diffuse") {
                ss << diffuseNr++;
            }
            else if(name == "texture_specular") {
                ss << specularNr++;
            }
            number = ss.str();
            
            glActiveTexture(GL_TEXTURE0 + i);
            glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        }
        
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (GLuint i = 0; i < m_textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW); //struct sequential memory layout

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);   
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
}

