#include "App.h"

#ifndef _INCLUDE_GLAD_
#define _INCLUDE_GLAD_
#include <glad/glad.h>
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#define __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#include <glm/gtc/matrix_transform.hpp>
#endif

#include <glm/gtc/type_ptr.hpp>

#ifndef __INCLUDE_STB_IMAGE__
#define __INCLUDE_STB_IMAGE__
#include "stb_image.h"
#endif

#include <iostream>

#include "Camera.h"
#include "Window.h"
#include "Physics.h"

#include "Shader.h"
#include "Model.h"
#include "Cube.h"
#include "Light.h"
#include "Plane.h"

void PrintVec3(glm::vec3 v)
{
    std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}


App::App(const int width, const int height, const std::string& title) :
    SCR_WIDTH(width), SCR_HEIGHT(height), TITLE(title), deltaTime(0.0f), lastFrame(0.0f),
    camera(nullptr), lightCube(nullptr), lightShader(nullptr),
    carModel(nullptr), modelShader(nullptr),
    road(nullptr),
    view(glm::mat4(0.f)), projection(glm::mat4(0.f))
{
	// Initialize GLFW window
    Window::Init(SCR_WIDTH, SCR_HEIGHT, TITLE);

    // Initialize PhysX API
    Physics::Init();

    // Init OpenGL and run
    InitOpenGL();
    LoadShaders();
    CreateDrawableObjects();

    // TODO: add error check before running
    Run();

    Window::Terminate();
    Physics::CleanUp();
}

App::~App()
{
    delete camera;
    delete carModel;
    delete modelShader;
}

// Set camera view and projection tranformations
void App::StartRender()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = camera->GetViewMatrix();
}

// Check all events and swap front and back buffers
void App::FinishRender()
{
    Window::PollEvents();
    Window::SwapBuffers();
}

int App::InitOpenGL()
{
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);


    // camera = new Camera(glm::vec3(3.0f, 1.0f, 3.0f));
    camera = new Camera(glm::vec3(0.0f, 2.0f, 20.0f));
    Window::RegisterCamera(camera);


    return 0;
}

void App::LoadShaders()
{
    lightShader = new Shader("shaders/lightSource_vshader.glsl", "shaders/lightSource_fshader.glsl");
    modelShader = new Shader("shaders/model_load_vshader.glsl", "shaders/model_load_fshader.glsl");
}

void App::CreateDrawableObjects()
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Load car model
    std::string modelPath = "assets/audi/audi.obj";
    carModel = new Model("car", glm::vec3(0.f, 5.f, 0.f), modelPath.data());

    // create light cube
    lightCube = new Light("light", glm::vec3(3.f, 20.f, 0.f));

    // create plane
    road = new Plane("plane");
}

void App::Run()
{
    // render loop
    glEnable(GL_DEPTH_TEST);
    while (!Window::ShouldClose())
    {
        // per-frame logic
        UpdateDeltaTime();

        // input
        Window::ProcessInput(deltaTime);

        // calculate physics
        Physics::Step(objectGlobalPoses, lightCube);

        StartRender();
        
        // don't forget to enable shader before setting uniforms
        modelShader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 300.0f));	// it's a bit too big for our scene, so scale it down

        modelShader->setMat4("model", model);
        modelShader->setMat4("projection", projection);
        modelShader->setMat4("view", view);
        
        modelShader->setVec3("lightAmbient", lightCube->Ambient);
        modelShader->setVec3("lightDiffuse", lightCube->Diffuse);
        modelShader->setVec3("lightSpecular", lightCube->Specular);
        modelShader->setVec3("lightColor", lightCube->Color);

        modelShader->setVec3("lightPosition", lightCube->Position);
        modelShader->setVec3("viewPos", camera->Position);

        // render plane
        road->Draw(*modelShader);

        // render the car
        model = glm::mat4(1.0f);
        model = model * objectGlobalPoses[carModel->Name];
        model = glm::translate(model, glm::vec3(0.0f, -0.875f, 0.0f)); // translate it down so it's at the center of the scene
        modelShader->setMat4("model", model);
        carModel->Draw(*modelShader);
        // modelShader->setMat4("model", objectGlobalPoses[carCube->Name]);
        // carCube->Draw(*modelShader);

        // render light source
        lightShader->use();
        lightShader->setMat4("model", objectGlobalPoses[lightCube->Name]);
        lightShader->setMat4("view", view);
        lightShader->setMat4("projection", projection);

        lightCube->Draw(*lightShader);
        // PrintVec3(lightCube->Position);

        FinishRender();
    }
}

void App::UpdateDeltaTime()
{
    float currentFrame = Window::GetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

