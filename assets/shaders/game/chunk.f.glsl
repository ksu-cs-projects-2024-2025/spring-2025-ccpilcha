#version 410 core
const ivec2 atlasSize = ivec2(128, 128);
const ivec2 tileSize = ivec2(8, 8);

uniform sampler2DArray textureAtlas;
uniform float opacity;
uniform vec3 hint;

in vec2 TexCoord;
flat in int layer;
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

    vec4 color = vec4(texture(textureAtlas, vec3(TexCoord, layer)))*vec4(faceColor,1.0);
    vec4 aoColor = mix(mix(color, vec4(0.0, 0.0, 0.0, 1.0), 0.5f), color, vAO);
    FragColor = vec4(mix(aoColor, vec4(0.53, 0.97, 1.0, 1.0), fogFactor));
}