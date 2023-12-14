#include "Window.h"

#include <iostream>


int Window::InternalInit(const int width, const int height)
{
    GetInstance()->m_width = width;
    GetInstance()->m_height = height;

    // Intialize and configure glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* m_window = glfwCreateWindow(m_width, m_height, "Lonely Driver", NULL, NULL);
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

bool Window::InternalShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::InternalSwapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GetInstance()->m_width  = width;
    GetInstance()->m_height = height;
    GetInstance()->lastX    = width / 2.f;
    GetInstance()->lastY    = height / 2.f;

    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Window::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (GetInstance()->firstMouse)
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
