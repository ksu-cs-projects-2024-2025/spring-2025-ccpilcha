#version 410 core

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Apply the model, view, and projection transformations
    vec3 aPos;
    if (gl_InstanceID == 0) {
        vertexColor = vec4(0.0, 1.0, 0.0, 1.0); // Green for +Z
        aPos = vec3(0.0, 0.0, float(gl_VertexID));
    } else if (gl_InstanceID == 1) {
        vertexColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for +X
        aPos = vec3(float(gl_VertexID), 0.0, 0.0);
    } else if (gl_InstanceID == 2) {
        vertexColor = vec4(0.0, 0.5, 1.0, 1.0); // Cyan for +Y
        aPos = vec3(0.0, float(gl_VertexID), 0.0);
    } else {
        vertexColor = vec4(1.0, 1.0, 1.0, 1.0); // Default color (white) for other directions
        aPos = vec3(0.0, 0.0, 0.0);
    }
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
}