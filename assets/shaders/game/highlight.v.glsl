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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int x;
uniform int y;
uniform int z;
uniform int face;

void main()
{
    // Calculate the vertex offset for this face and vertex
    vec3 offset = vertexOffsets[face* 4 + gl_VertexID];
    
    vec3 worldPos = (offset - vec3(0.5f,0.5f,0.5f)) * 1.01f + vec3(0.5f,0.5f,0.5f) + vec3(float(x), float(y), float(z));

	gl_Position = projection * view * model * vec4(worldPos, 1.0f);
}