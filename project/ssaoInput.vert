#version 420

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;

void main()
{
    vec4 viewSpacePos = modelViewMatrix * vec4(inPosition, 1.0);
    viewSpacePosition = viewSpacePos.xyz;
    viewSpaceNormal = mat3(modelViewMatrix) * inNormal;
    gl_Position = projectionMatrix * viewSpacePos;
}