#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Camera;

// Create singleton window object using glfw
class Window
{
public:
	Window(const Window&)      = delete; // delete copy constructor
	Window& operator=(Window&) = delete; // delete assignment operator

	static Window* GetInstance();
	
	static int	Init(const int width, const int height)	{ return GetInstance()->InternalInit(width, height); }
	static double GetTime()								{ return glfwGetTime(); }
	static bool ShouldClose()							{ return GetInstance()->InternalShouldClose(); }
	static void ProcessInput(Camera* camera, float deltaTime)			{ GetInstance()->InternalProcessInput(GetInstance()->m_window, camera, deltaTime); }
	static void PollEvents()							{ glfwPollEvents(); }
	static void SwapBuffers()							{ GetInstance()->InternalSwapBuffers(); }
	static void	Terminate()								{ glfwTerminate(); }
	static void PrintName() { std::cout << GetInstance()->m_name << std::endl; }

protected:
	Window() : m_window(nullptr), m_name("GLFW Window") {}

private:
	int  InternalInit(const int width, const int height);
	void InternalProcessInput(GLFWwindow* window, Camera* camera, float deltaTime);
	bool InternalShouldClose() const;
	void InternalSwapBuffers() const;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static Window* instancePtr;

	GLFWwindow* m_window;
	std::string m_name;
	int m_width = 800;
	int m_height = 600;

	float lastX = m_width / 2.0f;
	float lastY = m_height / 2.0f;
	bool  firstMouse = true;
};