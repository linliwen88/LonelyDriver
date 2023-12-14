#pragma once
#include <GLFW/glfw3.h>

// Create window using glfw
class Window
{
public:
	Window(const Window&)      = delete; // delete copy constructor
	Window& operator=(Window&) = delete; // delete assignment operator

	static Window* GetInstance()
	{
		static Window* instancePtr;
		return instancePtr;
	}

	static int Init(int width, int height) { return GetInstance()->InternalInit(width, height); }
	static int Terminate() { GetInstance()->InternalTerminate(); }

private:
	Window();
	int  InternalInit(int width, int height);
	void InternalTerminate();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* m_window;
	int m_width = 600;
	int m_height = 600;

	float lastX = m_width / 2.0f;
	float lastY = m_height / 2.0f;
	bool  firstMouse = true;
};
