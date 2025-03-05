#version 330 core

in vec4 vertexColor;
out vec4 FragColor;

void main()
{
    // Set the fragment color to a constant value
    FragColor = vertexColor;
}