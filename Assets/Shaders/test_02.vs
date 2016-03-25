#version 130

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0f);
    
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}
