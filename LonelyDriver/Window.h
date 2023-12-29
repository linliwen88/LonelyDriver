#pragma once

#ifndef _INCLUDE_GLFW_
#define _INCLUDE_GLFW_
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include <iostream> 

class Camera;

// Singleton window object using GLFW
class Window
{
public:
	static double	GetTime()		{ return glfwGetTime(); }
	static void		Terminate()		{ glfwTerminate(); std::cout << "glfw Terminated\n"; }
	static void		PollEvents()	{ glfwPollEvents(); }
	static bool		ShouldClose()	{ return glfwWindowShouldClose(m_window); }
	static void		SwapBuffers()	{		 glfwSwapBuffers(m_window); }
	static void		RegisterCamera(Camera* _camera) { camera = _camera; }

	static int	Init(int* scrWidthPtr, int* scrHeightPtr, const std::string& name);
	static void ProcessInput(float deltaTime);

private:
	Window() {}
	Window(const Window&)      = delete; // delete copy constructor
	Window& operator=(Window&) = delete; // delete assignment operator

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void error_callback(int error, const char* msg);

	static GLFWwindow*  m_window;
	static std::string	TITLE;
	static int*			SCR_WIDTH;
	static int*			SCR_HEIGHT;
	static float		lastX;
	static float		lastY;
	static bool			firstMouse;
	static Camera*		camera;
};