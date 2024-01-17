#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 offset = vec4((gl_InstanceID % 4) * 20.0, 0.0, (gl_InstanceID / 4) * 20.0, 0.0);
    offset = offset - vec4(40.0, 0.0, 40.0, 0.0);
    Normal = mat3(transpose(inverse(model))) * aNormal; // TODO: calculate normal matrix on CPU and send in as uniform
    FragPos = vec3(model * vec4(aPos, 1.0)); // fragment world-space position
    TexCoords = aTexCoords;    
    gl_Position = projection * view * ((model * vec4(aPos, 1.0)) + offset);
}