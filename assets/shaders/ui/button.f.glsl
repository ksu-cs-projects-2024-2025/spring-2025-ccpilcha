#version 410 core

uniform vec2 origin;
uniform vec2 radius;
uniform float borderWidth;
uniform vec4 mainColor; 
uniform vec4 highlightColor; 
uniform vec4 shadowColor; 
out vec4 fragColor;

void main() {
    vec2 relCoord = gl_FragCoord.xy - origin;     // Relative to center
    vec2 absDist = abs(relCoord);                  // Distance from center
    vec2 edgeDist = radius - absDist;             // Distance from edge

    bool isInsideBorder = edgeDist.x > borderWidth && edgeDist.y > borderWidth;

    if (isInsideBorder) {
        fragColor = vec4(mainColor);               // Fill
    } else {
        vec2 borderPos = (absDist - (radius - borderWidth)) / borderWidth;

        if (borderPos.x > borderPos.y)
            fragColor = vec4(highlightColor);
        else
            fragColor = vec4(shadowColor);
    }

    // Sample the button background texture
    //vec4 buttonColor = texture(buttonTexture, fragTexCoord);

    // Sample the text texture (assumes text is white on transparent background)
    //vec4 textColor = texture(textTexture, fragTexCoord);

    // Combine the button and text colors
    //fragColor = mix(buttonColor, textColor, textColor.a); // Blend text based on alpha
}
