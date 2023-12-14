#pragma once
class Window;

class App
{
public:
	App();
	~App();

	void Run();

private:
	int InitOpenGL();
	void LoadAssets();
	void ProcessInput(GLFWwindow* window);

	Window* window;

	int SCR_WIDTH;
	int SCR_HEIGHT;
};
