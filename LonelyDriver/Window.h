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

#ifndef __INCUDE_IMGUI__
#define __INCUDE_IMGUI__
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif // !__INCUDE_IMGUI__


#include <iostream> 
#include <iomanip>
#include <sstream>

class Camera;
struct Command;

// Singleton window object using GLFW
class Window
{
public:
	static double	GetTime()		{ return glfwGetTime(); }
	static void		Terminate();
	static void		PollEvents()	{ glfwPollEvents(); }
	static bool		ShouldClose()	{ return glfwWindowShouldClose(m_window); }
	static void		SwapBuffers()	{		 glfwSwapBuffers(m_window); }
	static void		RegisterCamera(Camera* _camera)
	{
		camera = _camera;

	}
	static void		StartGUIFrame();

	static void		RenderGUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	static void		SetWindowTitle(float fps) 
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << fps;
		glfwSetWindowTitle(m_window, (TITLE + "  FPS: " + ss.str()).c_str());
	}

	static int	Init(int* scrWidthPtr, int* scrHeightPtr, const std::string& name);
	static void ProcessInput(float deltaTime, Command& vehicleCommand);

private:
	Window() {}
	Window(const Window&)      = delete; // delete copy constructor
	Window& operator=(Window&) = delete; // delete assignment operator

	static int initGLFW();
	static void initIMGUI();
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