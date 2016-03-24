#version 130

attribute vec2 texcoord;
attribute vec3 v_color;
attribute vec3 position;

flat out vec3 f_color;
out vec2 f_texcoord;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    
    f_color = v_color;
    f_texcoord = texcoord;
}
