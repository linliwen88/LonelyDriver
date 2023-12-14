#include "App.h"
#include "Window.h"

App::App()
{
	// create GLFW window
	glfwWindow = Window::GetInstance();
}
