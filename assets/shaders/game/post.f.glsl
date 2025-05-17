#version 330 core
out vec4 FragColor;

in vec2 fragUV;

uniform float phase;
uniform float rippleStrength;
uniform vec3 hint;
uniform sampler2D screenTexture;

void main() {
    // Distortion via sine waves (or try noise)
    vec2 offset;
    offset.x = sin((fragUV.y) * 10.0 + phase) * rippleStrength;
    offset.y = cos((fragUV.x) * 10.0 + phase) * rippleStrength;

    vec2 distortedUV = fragUV + offset;
    vec3 color = texture(screenTexture, distortedUV).rgb;
    FragColor = vec4(color * hint, 1.0);
}
