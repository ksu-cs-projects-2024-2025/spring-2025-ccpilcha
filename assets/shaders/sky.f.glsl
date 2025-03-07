#version 410 core
in vec3 vDirection;
out vec4 FragColor;

void main() {
    // Use the y component of the normalized direction to blend colors.
    float t = clamp((vDirection.z + 1.0) * 0.5, 0.0, 1.0);
    // White at the horizon (t = 0) and darker blue at the zenith (t = 1)
    vec3 skyColor1 = mix(vec3(1.0), vec3(.53, .97, 1.0), smoothstep(0.0, 0.5, t));
    vec3 skyColor2 = mix(skyColor1, vec3(0.0, .53, 1.0), smoothstep(0.4, .8, t));
    FragColor = vec4(skyColor2, 1.0);
}