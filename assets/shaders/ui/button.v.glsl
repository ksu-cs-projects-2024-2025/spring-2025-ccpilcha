#version 410 core

uniform vec2 origin;
uniform vec2 radius;
uniform vec2 screenSize;


//out vec2 fragTexCoord; // Pass texture coordinates to the fragment shader

void main() {
    // Convert pixel coordinates to normalized device coordinates (NDC)
    float x = ((origin.x - radius.x) + float(2 * (gl_VertexID % 2)) * radius.x) / screenSize.x * 2.0 - 1.0;
    float y = ((origin.y - radius.y) + float(2 * (gl_VertexID / 2)) * radius.y) / screenSize.y * 2.0 - 1.0;

    gl_Position = vec4(x, y, 0.0, 1.0);
    //fragTexCoord = vec2(gl_VertexID % 2, gl_VertexID / 2); // Simple UV mapping
}