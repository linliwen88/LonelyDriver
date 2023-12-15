#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

// Create singleton window object using glfw
class Window
{
public:
	Window(int width, int height);

	Window(const Window&) = delete; // delete copy constructor
	Window& operator=(Window&) = delete; // delete assignment operator

	double	GetTime()		{ return glfwGetTime(); }
	bool	ShouldClose()	{ return glfwWindowShouldClose(m_window);  }
	void	PollEvents()	{ glfwPollEvents(); }
	void	SwapBuffers()	{ glfwSwapBuffers(m_window); }
	void	Terminate()		{ glfwTerminate(); }

private:

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* m_window;

	int m_width;
	int m_height;

	float lastX;
	float lastY;
	bool  firstMouse;
};