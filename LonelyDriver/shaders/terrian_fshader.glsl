#version 410 core

in float Height;
in vec2 TexCoord;
in vec4 lightSpacePostion;

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
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

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