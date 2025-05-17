#version 410 core
const float PI = 3.14159265359;

out vec4 FragColor;

uniform mat4 u_invProj;   // inverse projection matrix
uniform mat4 u_invView;   // inverse view matrix
uniform vec2 u_resolution; // screen resolution

void main() {
    // 1. Convert screen coords to Normalized Device Coordinates (NDC)
    vec2 ndc = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;
    vec4 clipSpacePos = vec4(ndc, -1.0, 1.0); // z = -1 for far plane

    // 2. Unproject to view space
    vec4 viewSpacePos = u_invProj * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;

    // 3. Unproject to world space direction
    vec4 worldPos = u_invView * viewSpacePos;
    vec3 dir = normalize(worldPos.xyz);

    // 4. Convert to spherical coordinates
    float theta = acos(clamp(dir.z, -1.0, 1.0));   // elevation
    float phi = atan(dir.y, dir.x);                // azimuth

    // 5. Normalize angles to 0–1
    vec2 angles = vec2(
        (phi + PI) / (2.0 * PI),   // u (horizontal)
        theta / PI                // v (vertical)
    );

    // 6. Use angles to compute color
    vec3 skyColor1 = mix(vec3(1.0), vec3(0.53, 0.97, 1.0), smoothstep(0.0, 0.8, 1-angles.y));
    vec3 skyColor2 = mix(skyColor1, vec3(0.0, 0.53, 1.0), smoothstep(0.4, 0.7, 1-angles.y));

    FragColor = vec4(skyColor2, 1.0);
}

/*
void main() {
    
    // Use the y component of the normalized direction to blend colors.
    float t = clamp((vDirection.z + 1.0) * 0.5, 0.0, 1.0);
    // White at the horizon (t = 0) and darker blue at the zenith (t = 1)
    vec3 skyColor1 = mix(vec3(1.0), vec3(.53, .97, 1.0), smoothstep(0.0, 0.5, t));
    vec3 skyColor2 = mix(skyColor1, vec3(0.0, .53, 1.0), smoothstep(0.4, .8, t));
    FragColor = vec4(skyColor2, 1.0);
}
*/