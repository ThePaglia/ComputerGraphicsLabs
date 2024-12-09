#version 420

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 normal = normalize(viewSpaceNormal);
    outColor = vec4(normal * 0.5 + 0.5, 1.0); // Map normal from [-1, 1] to [0, 1]
}