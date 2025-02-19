#version 410 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the vertex offset for this face and vertex
    vec3 offset = vertexOffsets[aFace* 6 + (gl_VertexID % 6)];
    vec2 texOffset = textureOffsets[aFace * 6 + (gl_VertexID % 6)];
    
    vec3 worldPos = chunkPos + aPos + offset;
    
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