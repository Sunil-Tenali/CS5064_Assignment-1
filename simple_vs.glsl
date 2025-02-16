#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;  // Still read but not used
        
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 object_color;
        
out vec3 ourColor;
        
void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
    ourColor = object_color;  // Use the uniform color instead of vertex attribute
}