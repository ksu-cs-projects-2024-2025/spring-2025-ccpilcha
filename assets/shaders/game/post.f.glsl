#version 330 core
out vec4 FragColor;

in vec2 fragUV;
uniform sampler2D screenTexture;

void main() {
    vec3 color = texture(screenTexture, fragUV).rgb;
    FragColor = vec4(color, 1.0);
}
