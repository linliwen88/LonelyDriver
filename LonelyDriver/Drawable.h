#pragma once
class Shader;

class Drawable
{
public:
	virtual ~Drawable() {}
	virtual void Draw(Shader& shader) = 0;
};
