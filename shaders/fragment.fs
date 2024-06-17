#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D Texture1;

void main()
{
    FragColor = texture(Texture1, TexCoord) * vec4(ourColor, 1.0);
}