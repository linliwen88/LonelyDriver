#include "Window.h"

#ifndef _INCLUDE_GLAD_
#define _INCLUDE_GLAD_
#include <glad/glad.h> // for glViewport()
#endif

#include "Camera.h"
#include <iostream>

GLFWwindow* Window::m_window    = nullptr;
std::string Window::TITLE       = "";
int*         Window::SCR_WIDTH   = nullptr;
int*         Window::SCR_HEIGHT  = nullptr;
float       Window::lastX       = 400.f / 2.0f;
float       Window::lastY       = 400.f / 2.0f;
bool        Window::firstMouse  = true;
Camera*     Window::camera      = nullptr;

int Window::Init(int* scrWidthPtr, int* scrHeightPtr, const std::string& title)
{
    SCR_WIDTH = scrWidthPtr;
    SCR_HEIGHT = scrHeightPtr;
    TITLE = title;

    lastX = *SCR_WIDTH / 2.f;
    lastY = *SCR_HEIGHT / 2.f;

    if (m_window == nullptr)
    {
        // register error callback for troubleshooting 
        glfwSetErrorCallback(Window::error_callback);

        // Intialize and configure glfw
        if (glfwInit() == GLFW_FALSE)
        {
            std::cout << "Failed to init glfw" << std::endl;
            return -1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        m_window = glfwCreateWindow(*SCR_WIDTH, *SCR_HEIGHT, TITLE.c_str(), NULL, NULL);

        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(m_window);

        glfwSetFramebufferSizeCallback(m_window, Window::framebuffer_size_callback);
        glfwSetCursorPosCallback(m_window, Window::mouse_callback);
        glfwSetScrollCallback(m_window, Window::scroll_callback);
    }

    return 0;
}

void Window::ProcessInput(float deltaTime)
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    *SCR_WIDTH  = width;
    *SCR_HEIGHT = height;
    lastX    = width / 2.f;
    lastY    = height / 2.f;
    glViewport(0, 0, *SCR_WIDTH, *SCR_HEIGHT);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Window::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Window::error_callback(int error, const char* msg)
{
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}
