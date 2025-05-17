#version 410 core

uniform vec2 origin;
uniform vec2 radius;
uniform float borderWidth;
uniform vec4 mainColor; 
uniform vec4 highlightColor; 
uniform vec4 shadowColor; 

uniform bool isHover;
uniform bool isClicked;

out vec4 fragColor;

void main() {
    vec2 relCoord = gl_FragCoord.xy - origin;     // Relative to center
    vec2 absDist = abs(relCoord);                  // Distance from center
    vec2 edgeDist = radius - absDist;             // Distance from edge

    bool isInside = edgeDist.x > borderWidth && edgeDist.y > borderWidth;

    int color;
    if (isInside) {
        color = int(isClicked);
    } else {
        // Which edge are we closer to?
        if (edgeDist.x < edgeDist.y) {
            // Closer to LEFT or RIGHT
            color = (relCoord.x > 0.0) ? 2 : int(!isClicked);
        } else {
            // Closer to TOP or BOTTOM
            color = (relCoord.y < 0.0) ? 2 : int(!isClicked);
        }
    }

    if (color == 0) fragColor = mainColor;
    if (color == 1) fragColor = highlightColor;
    if (color == 2) fragColor = shadowColor;
}
