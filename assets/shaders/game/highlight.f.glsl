#version 410 core

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform vec2 screenSize;
uniform float phase;
out vec4 FragColor;

//in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0., -1./3., 2./3., -1.);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)),
                d / (q.x + e),
                q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0, 4, 2), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    return c.z * mix(vec3(1.0), rgb, c.y);
}


void main()
{
    vec2 uv = gl_FragCoord.xy / screenSize;

    // Compare depth
    float sceneZ = texture(depthTexture, uv).r;
    float fragZ = gl_FragCoord.z;

    // Only render if we're in front of the scene
    if (fragZ > sceneZ + 0.0001) discard;

    vec3 color = texture(screenTexture, uv).rgb;

    vec3 hsv = rgb2hsv(color);
    hsv.x = mod(hsv.x + 0.5, 1.0);  // Shift hue by 180°
    vec3 inverted = hsv2rgb(hsv);

    FragColor = vec4(mix(inverted, vec3(1.0) - color, phase/2), 1.0);
}