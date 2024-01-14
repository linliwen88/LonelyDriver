#version 410 core

in float Height;
in vec2 TexCoord;

uniform sampler2D colorMap;

out vec4 FragColor;

void main()
{
    // float h = (Height + 16) / 64.0f;
    // FragColor = vec4(h, h, h, 1.0);

    vec3 color = texture(colorMap, TexCoord).xyz;
    FragColor = vec4(color, 1.0);
    // FragColor = vec4(Height, Height, Height, 1.0);
}