#version 410 core
layout (location = 0) in ivec3 aPos;
layout (location = 1) in int aBlockID;
layout (location = 2) in int aFace;

out vec2 TexCoord;
flat out int BlockID;
flat out int Face;
flat out int layer;

// Predefined vertex offsets for all faces
const vec3 vertexOffsets[36] = vec3[36](
    // -x face
    vec3(0, 1, 1), vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1),
    // +x face
    vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1), vec3(1, 1, 1), vec3(1, 0, 1), vec3(1, 0, 0),
    // -y face
    vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 0, 1), vec3(0, 0, 1), vec3(0, 0, 0),
    // +y face
    vec3(1, 1, 1), vec3(1, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1),
    // -z face
    vec3(1, 1, 0), vec3(1, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0),
    // +z face
    vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(1, 1, 1), vec3(0, 1, 1), vec3(0, 0, 1)
);

// Predefined texture coordinates for each vertex
const vec2 textureOffsets[36] = vec2[36](
    // -x face
    vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(0, 1), vec2(0, 0), vec2(1, 0),
    // +x face
    vec2(1, 1), vec2(0, 1), vec2(0, 0), vec2(0, 0), vec2(1, 0), vec2(1, 1),
    // -y face
    vec2(0, 1), vec2(1, 1), vec2(1, 0), vec2(1, 0), vec2(0, 0), vec2(0, 1),
    // +y face
    vec2(0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 1), vec2(1, 0), vec2(0, 0),
    // -z face
    vec2(0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 1), vec2(1, 0), vec2(0, 0),
    // +z face
    vec2(1, 1), vec2(0, 1), vec2(0, 0), vec2(0, 0), vec2(1, 0), vec2(1, 1)
);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the vertex offset for this face and vertex
    vec3 offset = vertexOffsets[aFace* 6 + (gl_VertexID % 6)];
    vec2 texOffset = textureOffsets[aFace * 6 + (gl_VertexID % 6)];

    vec3 worldPos = aPos + offset;
    
    if (aBlockID == 1)
    {
        if (aFace == 4) layer = 2;
        else if (aFace != 5) layer = 1;
        else layer = 0;
    } else {
        layer = aBlockID;
    }

	gl_Position = projection * view * model * vec4(worldPos, 1.0f);
	TexCoord = texOffset;
	BlockID = aBlockID;
	Face = aFace;
}