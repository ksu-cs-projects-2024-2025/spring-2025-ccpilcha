#version 410 core
const ivec2 atlasSize = ivec2(128, 128);
const ivec2 tileSize = ivec2(8, 8);

uniform sampler2DArray textureAtlas;
uniform float opacity;
uniform vec3 hint;

in vec2 TexCoord;
flat in int layer;
flat in int BlockID; 
flat in int Face;
in float vAO;
in float fogFactor;
out vec4 FragColor;

//in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    vec3 faceColor = hint;
    // temporary "shadow"
    if (Face == 4) faceColor = faceColor*2/5.f;
    else if (Face != 5) faceColor = faceColor*2/3.f;

    vec3 color = vec3(texture(textureAtlas, vec3(TexCoord, layer)))*faceColor;
    vec3 aoColor = mix(mix(color, vec3(0.f), 0.5f), color, vAO);
    FragColor = vec4(mix(aoColor, vec3(0.53, 0.97, 1.0), fogFactor), opacity);
}