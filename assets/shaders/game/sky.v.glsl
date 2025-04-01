#version 410 core

// Corrected vertex order for triangle strip
const vec3 vertexOffsets[24] = vec3[24](
    // -x face
    vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 1, 1), vec3(0, 0, 1),
    // +x face
    vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 0, 1), vec3(1, 1, 1),
    // -y face (Bottom)
    vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1),
    // +y face (Top)
    vec3(1, 1, 0), vec3(0, 1, 0), vec3(1, 1, 1), vec3(0, 1, 1),
    // -z face
    vec3(1, 1, 0), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 0),
    // +z face
    vec3(0, 0, 1), vec3(1, 0, 1), vec3(0, 1, 1), vec3(1, 1, 1)
);

out vec3 vDirection;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // We want the cube to be centered on the camera,
    // so we remove the camera's translation component.
    vec3 offset = vertexOffsets[gl_InstanceID * 4 + gl_VertexID] - vec3(0.5,0.5,0.5);
    mat4 viewNoTranslation = mat4(mat3(view));
    // Compute the vertex position
    vec4 pos = projection * viewNoTranslation * vec4(offset, 1.0);
    gl_Position = pos;
}
