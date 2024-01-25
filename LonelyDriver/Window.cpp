#include "Window.h"

#ifndef _INCLUDE_GLAD_
#define _INCLUDE_GLAD_
#include <glad/glad.h> // for glViewport()
#endif

#include "Camera.h"
#include "Physics.h"
#include <iostream>

GLFWwindow* Window::m_window    = nullptr;
std::string Window::TITLE       = "";
int*        Window::SCR_WIDTH   = nullptr;
int*        Window::SCR_HEIGHT  = nullptr;
float       Window::lastX       = 400.f / 2.0f;
float       Window::lastY       = 400.f / 2.0f;
bool        Window::firstMouse  = true;
Camera*     Window::camera      = nullptr;

void Window::Terminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate(); std::cout << "glfw Terminated\n";
}

void Window::StartGUIFrame(float* nearPlane, float* farPlane, float* leftPlane, float* rightPlane, float* topPlane, float* bottomPlane)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window = false;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        // Create a window called "Render setting" and append into it.
        ImGui::Begin("Render setting");

        //static ImGuiComboFlags flags = 0;
        //const char* items[] = { "Free", "Follow Car"};
        //int shading_mode = 0;
        //const char* combo_preview_value = items[shading_mode];  // Pass in the preview value visible before opening the combo (it could be anything)
        //if (ImGui::BeginCombo("Camera position", combo_preview_value, flags))
        //{
        //    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        //    {
        //        const bool is_selected = (shading_mode == n);
        //        if (ImGui::Selectable(items[n], is_selected))
        //            shading_mode = n;

        //        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        //        if (is_selected)
        //            ImGui::SetItemDefaultFocus();
        //    }
        //    ImGui::EndCombo();
        //}

        ImGui::Text("depthMap render frustrum");               // Display some text (you can use a format strings too)
        //depthMap render space:
        //nearPlane
        //    farPlane
        //    left
        //    right bottom top
        ImGui::SliderFloat("Near Plane", nearPlane, 0.1f, 100.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Far Plane", farPlane, 10.f, 1000.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Left Plane", leftPlane, -1.0f, -200.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Right Plane", rightPlane, 1.0f, 200.f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Bottom Plane", bottomPlane, -1.0f, -200.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Top Plane", topPlane, 1.0f, 200.f);     // Edit 1 float using a slider from 0.0f to 1.0f
        bool lastCameraFollow = camera->GetFollowSwitch();
        ImGui::Checkbox("Camera Follow Car", &camera->GetFollowSwitch());
        //if (lastCameraFollow != camera->GetFollowSwitch())
        //{
        //    // camera follow changed
        //    printf("camera follow changed\n");
        //    camera->UpdateAttributes();
        //}
        ImGui::End();
    }
}

int Window::Init(int* scrWidthPtr, int* scrHeightPtr, const std::string& title)
{
    SCR_WIDTH = scrWidthPtr;
    SCR_HEIGHT = scrHeightPtr;
    TITLE = title;

    lastX = *SCR_WIDTH / 2.f;
    lastY = *SCR_HEIGHT / 2.f;

    if (m_window == nullptr)
    {
        initGLFW();
    }

    initIMGUI();

    return 0;
}

void Window::ProcessInput(float deltaTime, Command& vehicleCommand)
{
    // ----------------
    // camera control
    // ----------------
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

    // ----------------
    // vehicle control
    // ----------------
    // accelerate (throttle)
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        vehicleCommand.throttle = 1.0;
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_RELEASE)
        vehicleCommand.throttle = 0.0;

    // brake
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        vehicleCommand.throttle = 0.0;
        vehicleCommand.brake = 0.5;
    }
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_RELEASE)
    {
        vehicleCommand.brake = 0.0;
    }

    // turn
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        vehicleCommand.steer = 0.5;
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        vehicleCommand.steer = -0.5;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_RELEASE && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
        vehicleCommand.steer = 0.0;

}

int Window::initGLFW()
{
    // register error callback for troubleshooting 
    glfwSetErrorCallback(Window::error_callback);

    // Intialize and configure glfw
    if (glfwInit() == GLFW_FALSE)
    {
        std::cout << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

void Window::initIMGUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 410";
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
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
