#version 410 core
layout (location = 0) in uint packed0;
layout (location = 1) in uint packed1;
uvec3 aPos;
uint aTextureID;
uint aFace;

out vec2 TexCoord;
flat out int TextureID;
flat out int Face;
flat out int layer;
out float vAO;
out float fogFactor;

// Corrected vertex order for triangle strip
const vec3 vertexOffsets[24] = vec3[24](
    // -x face
    vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 1, 1), vec3(0, 0, 1),
    // +x face
    vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 0, 1), vec3(1, 1, 1),
    // -y face
    vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1),
    // +y face
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
uniform vec3 plrPos;
uniform vec3 size;
uniform vec3 offset;
uniform int LOD;

void main()
{
    aPos.x = bitfieldExtract(packed0, 27, 5);
    aPos.y = bitfieldExtract(packed0, 22, 5);
    aPos.z = bitfieldExtract(packed0, 17, 5);
    aFace = bitfieldExtract(packed0, 14, 3);
    aTextureID = bitfieldExtract(packed0, 0, 14) << 2 | bitfieldExtract(packed1, 30, 2);
    uint aoBits = (packed1 >> 22u) & 0xFFu;
    uint ao = (aoBits >> ((gl_VertexID % 4) * 2)) & 0x3u;
    vAO = 0.25 + float(ao) * 0.25;

    // Calculate the vertex offset for this face and vertex
    vec3 offset = vertexOffsets[aFace* 4 + gl_VertexID];
    vec2 texOffset = textureOffsets[gl_VertexID];
    
    vec3 worldPos = chunkPos + aPos + offset;

    fogFactor = max(0.f, (length(worldPos - plrPos))/2000.f);
	gl_Position = projection * view * model * vec4(worldPos, 1.0f);
	TexCoord = texOffset;
	layer = int(aTextureID);
	Face = int(aFace);
}