#version 410 core

in float Height;
in vec2 TexCoord;
in vec4 Normal;
in vec4 lightSpacePostion;
in vec4 worldSpaceFragPos;

uniform vec3 lightPosition;
uniform sampler2D colorMap;
uniform sampler2D shadowMap;

out vec4 FragColor;

float ShaderCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 lightDir = lightPosition - worldSpaceFragPos.xyz;
    float bias = max(0.0005 * (1.0 - dot(Normal.xyz, lightDir)), 0.0001); // TODO: adjust bias to eliminate shadow acne
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    // float h = (Height + 16) / 64.0f;
    // FragColor = vec4(h, h, h, 1.0);

    vec3 color = texture(colorMap, TexCoord).xyz;
    float shadow = ShaderCalculation(lightSpacePostion);
    FragColor = (shadow == 1.0)?  0.5 * vec4(color, 1.0) : vec4(color, 1.0);
    // FragColor = vec4(Height, Height, Height, 1.0);
}