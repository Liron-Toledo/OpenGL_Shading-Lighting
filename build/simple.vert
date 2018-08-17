#version 330 core

in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;

void main()
{
    vec4 transformedPosition = projectionMatrix*viewingMatrix*modelMatrix*vec4(position,1.0f);
    gl_Position = transformedPosition;
}
