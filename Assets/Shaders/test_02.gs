#version 130

layout(triangles) in;
 
// Three lines will be generated: 6 vertices
layout(line_strip, max_vertices=6) out;

float normal_length = 10.0f;

in mat4 mvp;
in vec3 Normal[];

        vec3 P = gl_Position.xyz;
        vec3 N = Normal[i].xyz;
        
        gl_Position = mvp * vec4(P, 1.0);
        vertex_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        EmitVertex();
        
        gl_Position = mvp * vec4(P + N * normal_length, 1.0);
        vertex_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        EmitVertex();
        
        EndPrimitive();
    }
}
