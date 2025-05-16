#version 330 core

// Inputs
in vec2 UV;

// Outputs
out vec4 colour;

// Uniforms
uniform sampler2D textureMap;

void main()
{
    colour = vec4(texture(textureMap, UV));
}