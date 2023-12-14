#pragma once
class Window;

class App
{
public:
	App();
	~App();

	void Run();

private:
	void InitOpenGL();
	void LoadAssets();

	Window* glfwWindow;

	int SCR_WIDTH;
	int SCR_HEIGHT;
};
