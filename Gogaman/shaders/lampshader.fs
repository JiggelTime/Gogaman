#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec3 lightColor = vec3(0.5f, 0.9f, 0.6f);

void main()
{
    FragColor = vec4(lightColor, 1.0f);
}