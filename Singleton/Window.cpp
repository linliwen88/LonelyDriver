#include "Window.h"
#include <iostream>

Window::instance = NULL;

Window::Window(int width = 800, int height = 600) : 
    m_window(nullptr), m_width(width), m_height(height), lastX(width / 2.0f), lastY(height / 2.0f), firstMouse(true)
{
    // Intialize and configure glfw
    if (glfwInit() == GLFW_FALSE)
    {
        std::cout << "Failed to init glfw" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    m_window = glfwCreateWindow(width, height, "Lonely Driver", NULL, NULL);

    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    glfwSetFramebufferSizeCallback(m_window, Window::framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, Window::mouse_callback);
    glfwSetScrollCallback(m_window, Window::scroll_callback);

}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    m_width = width;
    m_height = height;
    GetInstance()->lastX = width / 2.f;
    GetInstance()->lastY = height / 2.f;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Window::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        GetInstance()->lastX = xpos;
        GetInstance()->lastY = ypos;
        GetInstance()->firstMouse = false;
    }

    float xoffset = xpos - GetInstance()->lastX;
    float yoffset = GetInstance()->lastY - ypos; // reversed since y-coordinates go from bottom to top

    GetInstance()->lastX = xpos;
    GetInstance()->lastY = ypos;

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
