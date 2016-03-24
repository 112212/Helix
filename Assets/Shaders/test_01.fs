#version 130

//uniform vec3 outColor;
flat in vec3 f_color;
in vec2 f_texcoord;

uniform sampler2D mytexture;

void main() {
    //gl_FragColor = vec4(f_color, 1.0f);
    
    vec2 flipped_texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
    gl_FragColor = texture2D(mytexture, flipped_texcoord);
}
