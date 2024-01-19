#version 410 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

out vec2 TexCoords;

void main()
{
	TexCoords = vec2((aPos.x + 1.0) / 2.0, (aPos.z + 1.0) / 2.0);
	gl_Position = vec4(aPos.x, aPos.z, 0.0, 1.0);
}