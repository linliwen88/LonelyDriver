#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform sampler2D heightMap;
uniform int terrainTexWidth;
uniform int terrainTexHeight;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    float spacing = 15.0;
    vec4 offset = vec4(((gl_InstanceID % 6) * spacing) - (2.0 * spacing), 0.0, ((gl_InstanceID / 6) * spacing) - (2.0 * spacing), 0.0);
    worldPosition = worldPosition + offset;

    float terrainTexWidth_float = float(terrainTexWidth);
    float terrainTexHeight_float = float(terrainTexHeight);

    vec2 heightTexCoord = vec2((worldPosition.x + (terrainTexWidth_float  / 2.0)) / terrainTexWidth_float, 
                               (worldPosition.z + (terrainTexHeight_float / 2.0)) / terrainTexHeight_float);

    float height = texture(heightMap, heightTexCoord).x * 64.0 - 8.0;
    worldPosition = worldPosition + vec4(0.0, height, 0.0, 0.0);

    Normal = mat3(transpose(inverse(model))) * aNormal; // TODO: calculate normal matrix on CPU and send in as uniform
    FragPos = vec3(model * vec4(aPos, 1.0)); // fragment world-space position
    TexCoords = aTexCoords;    
    gl_Position = projection * view * worldPosition;
}