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

float ShadowCalculation(vec4 fragPosLightSpace)
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
    float bias = max(0.0005 * (1.0 - dot(Normal.xyz, lightDir)), 0.0001); // TODO: tweak bias min max value to eliminate shadow acne

    // PCF (percentage-closer filtering)
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    vec3 color = texture(colorMap, TexCoord).xyz;
    float shadow = ShadowCalculation(lightSpacePostion);
    // TODO: render terrain with specular, diffuse, ambient lighting
    FragColor = (shadow >= 0.1)? (1.0 - (shadow * 0.7)) * vec4(color, 1.0) : vec4(color, 1.0);
}