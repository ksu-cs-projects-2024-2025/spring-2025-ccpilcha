#version 330 core

layout(location = 0) in vec2 aPos;      // [0,1] unit quad
layout(location = 1) in vec2 aUV;

layout(location = 2) in vec2 iOffset;   // Instance: screen-space position
layout(location = 3) in vec2 iSize;     // Instance: width and height
layout(location = 4) in vec2 iUVMin;    // Instance: glyph uv min
layout(location = 5) in vec2 iUVMax;    // Instance: glyph uv max

out vec2 vUV;

uniform vec2 uScreenSize; // Viewport resolution in pixels

void main() {
    vec2 pos = iOffset + aPos * iSize;
    vec2 ndc = (pos / uScreenSize) * 2.0 - 1.0;
    gl_Position = vec4(ndc.x, ndc.y, 0.0, 1.0); // Flip Y

    vUV = mix(iUVMin, iUVMax, aUV);
}
