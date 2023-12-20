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

#include "Shader.h"
#include "Camera.h"
#include "Window.h"
#include "Model.h"
#include "Cube.h"
#include <iostream>

App::App(const int width, const int height, const std::string& title) :
    SCR_WIDTH(width), SCR_HEIGHT(height), TITLE(title), deltaTime(0.0f), lastFrame(0.0f),
    camera(nullptr),
    lightPosition(glm::vec3(1.0)), lightShader(nullptr),
    carModel(nullptr), modelShader(nullptr),
    lightCube(nullptr)
{
	// create GLFW window
    Window::Init(SCR_WIDTH, SCR_HEIGHT, TITLE);

    // Init OpenGL and run
    InitOpenGL();
    LoadModels();

    // TODO: add error check before running
    Run();

    Window::Terminate();
}

App::~App()
{
    delete camera;
    delete carModel;
    delete modelShader;
}

int App::InitOpenGL()
{
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    camera = new Camera(glm::vec3(3.0f, 1.0f, 3.0f));
    Window::RegisterCamera(camera);

    lightCube = new Cube();
    lightShader = new Shader("shaders/lightSource_vshader.glsl", "shaders/lightSource_fshader.glsl");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return 0;
}

void App::LoadModels()
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    modelShader = new Shader("shaders/model_load_vshader.glsl", "shaders/model_load_fshader.glsl");

    // std::string modelPath = "assets/backpack/backpack.obj";
    std::string modelPath = "assets/audi/audi.obj";
    carModel = new Model(modelPath.data());
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

        glClearColor(0.05f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        modelShader->use();
        lightPosition = glm::vec3(2.2f, 2.0f, 2.0f);
        glm::vec3 lightAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        modelShader->setMat4("projection", projection);
        modelShader->setMat4("view", view);
        modelShader->setMat4("model", model);
        
        modelShader->setVec3("lightAmbient", lightAmbient);
        modelShader->setVec3("lightAmbient", lightAmbient);
        modelShader->setVec3("lightDiffuse", lightDiffuse);
        modelShader->setVec3("lightSpecular", lightSpecular);
        modelShader->setVec3("lightColor", lightColor);

        modelShader->setVec3("lightPosition", lightPosition);
        modelShader->setVec3("viewPos", camera->Position);



        // render the loaded model
        carModel->Draw(*modelShader);

        lightShader->use();
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader->setMat4("model", model);
        lightShader->setMat4("view", view);
        lightShader->setMat4("projection", projection);
        lightCube->Draw(*lightShader);



        // check all events and swap the buffers
        Window::PollEvents();
        Window::SwapBuffers();
    }
}

void App::UpdateDeltaTime()
{
    float currentFrame = Window::GetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

