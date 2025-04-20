#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uFontAtlas;
uniform vec4 uTextColor;
uniform float uPxRange; // Must match the distance range used in atlas-gen

// Standard msdf edge detection
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 sdf = texture(uFontAtlas, vUV).rgb;
    float sigDist = median(sdf.r, sdf.g, sdf.b);
    
    // Convert SDF distance to alpha
    float screenPxDist = uPxRange * (sigDist - 0.5);
    float alpha = clamp(screenPxDist + 0.5, 0.0, 1.0);

    FragColor = vec4(uTextColor.rgb, uTextColor.a * alpha);
}
