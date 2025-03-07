#version 410 core
layout (location = 0) in uint data;
uvec3 aPos;
uint aBlockID;
uint aFace;

out vec2 TexCoord;
flat out int BlockID;
flat out int Face;
flat out int layer;

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

// Corrected texture coordinates
const vec2 textureOffsets[4] = vec2[4](
    // -x face
    vec2(0, 1), vec2(1, 1), vec2(0, 0), vec2(1, 0)
);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 chunkPos;
uniform int LOD;

void main()
{
    aPos.x = bitfieldExtract(data, 27, 5);
    aPos.y = bitfieldExtract(data, 22, 5);
    aPos.z = bitfieldExtract(data, 17, 5);
    aFace = bitfieldExtract(data, 14, 3);
    aBlockID = bitfieldExtract(data, 0, 14);
    // Calculate the vertex offset for this face and vertex
    vec3 offset = vertexOffsets[aFace* 4 + gl_VertexID];
    vec2 texOffset = textureOffsets[gl_VertexID];
    
    vec3 worldPos = chunkPos + aPos + offset;
    
    if (aBlockID == 1)
    {
        if (aFace == 4) layer = 2;
        else if (aFace != 5) layer = 1;
        else layer = 0;
    } else {
        layer = int(aBlockID);
    }

	gl_Position = projection * view * model * vec4(worldPos, 1.0f);
	TexCoord = texOffset;
	BlockID = int(aBlockID);
	Face = int(aFace);
}