#version 410 core

in vec4 fColor;
out vec4 FragColor;

void main()
{
    // Set the fragment color to a constant value
    FragColor = fColor;
}