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

#include "Window.h"
#include "Physics.h"

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Cube.h"
#include "Skybox.h"
#include "Light.h"
#include "Plane.h"

void PrintVec3(glm::vec3 v)
{
    std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}

App::App(const int width, const int height, const std::string& title) :
    SCR_WIDTH(width), SCR_HEIGHT(height), TITLE(title), deltaTime(1.0f / 60.0f), lastTime(0.0f),
    camera(nullptr), lightCube(nullptr), lightShader(nullptr),
    carModel(nullptr), modelShader(nullptr),
    road(nullptr),
    view(glm::mat4(0.f)), projection(glm::mat4(0.f)),
    DrawWireframe(false)
{
	// Initialize GLFW window
    Window::Init(&SCR_WIDTH, &SCR_HEIGHT, TITLE);

    // Initialize PhysX API
    Physics::Init();

    // Init OpenGL and run
    InitOpenGL();

    // create skybox
    CreateSkybox();

    // create terrain
    CreateTerrain();

    CreateDrawableObjects();

    // TODO: add error check before running
    Run();

    Window::Terminate();
    Physics::CleanUp();

    delete this;
}

App::~App()
{
    delete camera;
    delete modelShader;
    delete lightShader;

    delete carModel;
    delete lightCube;
    delete road;
    delete skybox;
}

void App::UpdateDeltaTimeAndPhysics()
{
    // Semi - fixed timestep
    float newTime = Window::GetTime();
    float FrameTime = newTime - lastTime;
    deltaTime = FrameTime;
    lastTime = newTime;

    // update physics with constant tick rate
    while (FrameTime > 0.f)
    {
        float physics_DeltaTime = fmin(FrameTime, physics_StepTime);

        // calculate physics, update object world poses (position and rotation) and light position
        Physics::Step(physics_DeltaTime, objectGlobalPoses, lightCube);

        FrameTime -= physics_DeltaTime;
    }

    // display FPS
    if (oneSec > 1.0f)
    {
        Window::SetWindowTitle(frameCount / oneSec);
        oneSec = 0.0f;
        frameCount = 1;
    }
    else
    {
        oneSec += deltaTime;
        frameCount++;
    }
}

// Set camera view and projection tranformations
void App::StartRender()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    view = camera->GetViewMatrix();
}

// Check all events and swap front and back buffers, update delta time for frame-dependent physics
void App::FinishRender()
{
    float pauseTimeStart = Window::GetTime();
    
    Window::PollEvents();
    Window::SwapBuffers();

    float puaseTime = Window::GetTime() - pauseTimeStart;
    if (puaseTime > 0.2f) lastTime += puaseTime;
}

int App::InitOpenGL()
{
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // create camera
    camera = new Camera(glm::vec3(0.0f, 2.0f, 20.0f));
    Window::RegisterCamera(camera);

    return 0;
}

void App::CreateSkybox()
{
    skyboxShader = new Shader("shaders/skybox_vshader.glsl", "shaders/skybox_fshader.glsl");
    skybox = new Skybox("skybox");
}

void App::CreateTerrain()
{
    tessHeightMapShader = new Shader("shaders/terrian_vshader.glsl", "shaders/terrian_fshader.glsl", nullptr, "shaders/terrian_tcshader.glsl", "shaders/terrian_teshader.glsl");

    // load height map texture
    glGenTextures(1, &terrainTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters, didn't use mipmap for heightmap because Mip levels won't be selected automatically with derivatives like in the fragment shader. (learn OpenGL)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nChannels;
    unsigned char* data = stbi_load("assets/heightmap.png", &width, &height, &nChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        tessHeightMapShader->setInt("heightMap", 0);
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // vertex generation
    std::vector<float> patchVertices;

    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            patchVertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            patchVertices.push_back(i / (float)rez); // u
            patchVertices.push_back(j / (float)rez); // v

            patchVertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            patchVertices.push_back((i + 1) / (float)rez); // u
            patchVertices.push_back(j / (float)rez); // v

            patchVertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            patchVertices.push_back(i / (float)rez); // u
            patchVertices.push_back((j + 1) / (float)rez); // v

            patchVertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            patchVertices.push_back((i + 1) / (float)rez); // u
            patchVertices.push_back((j + 1) / (float)rez); // v
        }
    }
    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    printf("Max supported patch vertices %d\n", MaxPatchVertices);
    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * patchVertices.size(), &patchVertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
}

void App::CreateDrawableObjects()
{
    // load shaders
    lightShader = new Shader("shaders/lightSource_vshader.glsl", "shaders/lightSource_fshader.glsl");
    modelShader = new Shader("shaders/model_load_vshader.glsl", "shaders/model_load_fshader.glsl");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // Load car model
    std::string modelPath = "assets/good-dirty-car/car.fbx";
    carModel = new Model("car", glm::vec3(0.f, 5.f, 0.f), modelPath.data());

    // create light cube
    lightCube = new Light("light", glm::vec3(3.f, 10.f, 0.f));

    // create plane
    road = new Plane("plane");
}

void App::DrawTerrain()
{
    tessHeightMapShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    tessHeightMapShader->setMat4("model", model);
    tessHeightMapShader->setMat4("view", view);
    tessHeightMapShader->setMat4("projection", projection);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(terrainVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void App::Run()
{
    // render loop
    lastTime = Window::GetTime();
    glEnable(GL_DEPTH_TEST);

    while (!Window::ShouldClose())
    {
        UpdateDeltaTimeAndPhysics();

        // input
        static int carDirection = 0;
        Window::ProcessInput(deltaTime, carDirection);

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

        DrawWireframe = true;
        // render plane
        road->Draw(*modelShader, DrawWireframe);

        // render the car
        // model = glm::mat4(1.0f);
        model = objectGlobalPoses[carModel->Name];
        carModel->Draw(*modelShader, model, DrawWireframe, carDirection);

        // render light source
        lightShader->use();
        lightShader->setMat4("model", objectGlobalPoses[lightCube->Name]);
        lightShader->setMat4("view", view);
        lightShader->setMat4("projection", projection);
        lightCube->Draw(*lightShader, DrawWireframe);
        // PrintVec3(lightCube->Position);

        // render terrain
        // DrawTerrain();

        // render skybox last for special view matrix
        skyboxShader->use();
        view = glm::mat4(glm::mat3(view)); // remove translation section, only keep rotation section of camera
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        skybox->Draw(*skyboxShader, DrawWireframe);

        FinishRender();
    }
}

