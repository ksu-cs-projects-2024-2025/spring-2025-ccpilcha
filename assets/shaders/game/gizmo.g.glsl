#version 410 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

in vec4 vertexColor[];         // Array, because we're dealing with lines
out vec4 fColor;            // Pass to fragment shader

uniform vec2 u_viewportSize;    // screen resolution in pixels
uniform float u_thickness;

void emitQuad(vec4 p0, vec4 p1, vec2 normal, float halfWidth, vec4 color, float zOffset) {
    vec2 offset = normal * halfWidth;

    fColor = color;
    gl_Position = vec4(p0.xy + offset, p0.z + zOffset, p0.w);
    EmitVertex();

    fColor = color;
    gl_Position = vec4(p0.xy - offset, p0.z + zOffset, p0.w);
    EmitVertex();

    fColor = color;
    gl_Position = vec4(p1.xy + offset, p1.z + zOffset, p1.w);
    EmitVertex();

    fColor = color;
    gl_Position = vec4(p1.xy - offset, p1.z + zOffset, p1.w);
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir    = normalize((p2.xy/p2.w - p1.xy/p1.w) * u_viewportSize);
    vec2 normal = vec2(-dir.y, dir.x) * u_thickness / u_viewportSize;

     // First: black outline
    emitQuad(p1, p2, normal, u_thickness * 1.5, vec4(0.0, 0.0, 0.0, 1.0), 0.15);

    // Then: actual line fill
    emitQuad(p1, p2, normal, u_thickness, vertexColor[0], 0.1); // Replace with your actual color logic

}
