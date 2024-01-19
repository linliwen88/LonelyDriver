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
#include "Vehicle.h"
#include "Skybox.h"
#include "Light.h"
#include "Plane.h"

void PrintVec3(glm::vec3 v)
{
    std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}

void PrintVec4(glm::vec4 v)
{
    std::cout << v.x << ' ' << v.y << ' ' << v.z << v.w << std::endl;
}

App::App(const int width, const int height, const std::string& title) :
    SCR_WIDTH(width), SCR_HEIGHT(height), TITLE(title), 
    deltaTime(1.0f / 60.0f), lastTime(0.0f), physicsTimeCounter(0.0f),
    camera(nullptr), lightSource(nullptr), lightShader(nullptr),
    tree(nullptr), carModel(nullptr), modelShader(nullptr),
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

    InitShadowMapping();

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
    printf("delete shaders\n");

    delete carModel;
    delete lightSource;
    delete road;
    delete skybox;
}

void App::InitShadowMapping()
{
    // generate FBO for depth map rendering
    GenerateFrameBuffer();

    // set shader to render depth map
    depthShader = new Shader("shaders/depthMap_vshader.glsl", "shaders/depthMap_fshader.glsl");
}


void App::UpdateDeltaTimeAndPhysics()
{
    // Semi - fixed timestep
    float newTime = Window::GetTime();
    deltaTime = newTime - lastTime;
    lastTime = newTime;

    physicsTimeCounter += deltaTime;

    // update physics with constant tick rate
    while (physicsTimeCounter > physicsStepTime)
    {
        // calculate physics, update object world poses (position and rotation) and light position
        Physics::Step(physicsStepTime, objectGlobalPoses, carModel->GetPosition(), carModel->GetRotation());
        physicsTimeCounter -= physicsStepTime;
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
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    view = camera->GetViewMatrix();
    viewSkybox = glm::mat4(glm::mat3(view));
}

// render the entire scence with designated shader or 
// render with specialized shader (when parameter shader is nullptr) for each drawable objects
void App::RenderScene(Shader* shader)
{
    if (shader)
    {
        shader->use();
        shader->setInt("isTree", 0);
        DrawWireframe = false;
        glm::mat4 model = glm::mat4(1.0f);

        // draw terrain
        DrawTerrain(shader);

        // draw road
        model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        // model = glm::scale(model, glm::vec3(5.0f, 1.0f, 300.0f));	// it's a bit too big for our scene, so scale it down
        shader->setMat4("model", model);
        road->Draw(*shader, DrawWireframe);

        // draw vehicle
        model = objectGlobalPoses[carModel->Name];
        carModel->Draw(*shader, model, DrawWireframe, Physics::getVehicleCommand().steer);

        // draw trees
        model = glm::mat4(1.0f);
        shader->setMat4("model", model);
        shader->setInt("terrainTexWidth", terrainTexWidth);
        shader->setInt("terrainTexHeight", terrainTexHeight);
        shader->setInt("heightMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainHeightTexture);

        shader->setInt("isTree", 1);
        // glDepthMask(GL_FALSE); // make the z-buffer read-only
        tree->Draw(*shader, model, Physics::getVehicleCommand().steer, 10);
        // glDepthMask(GL_TRUE); // set the depth mask bakc to read-write

    }
    else
    {
        // render skybox last for special view matrix
        skyboxShader->use();
        skyboxShader->setMat4("view", viewSkybox);
        skyboxShader->setMat4("projection", projection);
        skybox->Draw(*skyboxShader, DrawWireframe);

        // draw models 
        // don't forget to enable shader before setting uniforms
        DrawWireframe = false;

        // render terrain
        DrawTerrain(tessShader);
        modelShader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        // model = glm::scale(model, glm::vec3(5.0f, 1.0f, 300.0f));	// it's a bit too big for our scene, so scale it down

        modelShader->setMat4("model", model);
        modelShader->setMat4("projection", projection);
        modelShader->setMat4("view", view);

        modelShader->setVec3("lightColor", lightSource->Color);
        modelShader->setVec3("lightAmbient", lightSource->Ambient);
        modelShader->setVec3("lightDiffuse", lightSource->Diffuse);
        modelShader->setVec3("lightSpecular", lightSource->Specular);
        modelShader->setVec3("lightPosition", lightSource->Position);
        modelShader->setVec3("viewPos", camera->Position);

        // render road
        road->Draw(*modelShader, DrawWireframe);

        // render the car
        modelShader->setInt("modelType", 0);
        model = objectGlobalPoses[carModel->Name];
        carModel->Draw(*modelShader, model, DrawWireframe, Physics::getVehicleCommand().steer);


        modelTreeShader->use();
        model = glm::mat4(1.0f);
        modelTreeShader->setMat4("model", model);
        modelTreeShader->setMat4("projection", projection);
        modelTreeShader->setMat4("view", view);

        modelTreeShader->setVec3("lightColor", lightSource->Color);
        modelTreeShader->setVec3("lightAmbient", lightSource->Ambient);
        modelTreeShader->setVec3("lightDiffuse", lightSource->Diffuse);
        modelTreeShader->setVec3("lightSpecular", lightSource->Specular);
        modelTreeShader->setVec3("lightPosition", lightSource->Position);
        modelTreeShader->setVec3("viewPos", camera->Position);

        modelTreeShader->setInt("terrainTexWidth", terrainTexWidth);
        modelTreeShader->setInt("terrainTexHeight", terrainTexHeight);
        modelTreeShader->setInt("heightMap", 10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, terrainHeightTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // render tree after solid object are drawn
        glEnable(GL_BLEND); // enable color blending 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glDepthMask(GL_FALSE); // make the z-buffer read-only
        modelTreeShader->setInt("modelType", 1);
        tree->Draw(*modelTreeShader, model, Physics::getVehicleCommand().steer, 10);
        // glDepthMask(GL_TRUE); // set the depth mask bakc to read-write 
        glDisable(GL_BLEND); // disable blending
    }
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

void App::RenderDepthMap()
{
    // 1. first render to depth map
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // configure shader and matrices
    depthShader->use();


    float near_plane = 0.1f, far_plane = 10000.0f;
    // glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightProjection = glm::ortho(leftPlane, rightPlane, bottomPlane, topPlane, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightSource->Position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;

    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    // glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // render scene from light source perspective
    RenderScene(depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::ShowDepthMap()
{
    displayDepthMapShader->use();

    displayDepthMapShader->setInt("depthMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);

    canvas->Draw(*displayDepthMapShader, false);

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
    camera = new Camera(glm::vec3(10.0f, 5.0f, 10.0f));
    Window::RegisterCamera(camera);

    // create light
    lightSource = new Light(glm::vec3(1.f, 100.f, 1.f));

    return 0;
}

void App::CreateSkybox()
{
    skyboxShader = new Shader("shaders/skybox_vshader.glsl", "shaders/skybox_fshader.glsl");
    skybox = new Skybox("skybox");
}

void App::CreateTerrain()
{
    tessShader = new Shader("shaders/terrian_vshader.glsl", "shaders/terrian_fshader.glsl", nullptr, "shaders/terrian_tcshader.glsl", "shaders/terrian_teshader.glsl");
    tessShader->use();
    // ----------------------
    // load height map texture
    // ----------------------
    glGenTextures(1, &terrainHeightTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainHeightTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters, didn't use mipmap for heightmap because Mip levels won't be selected automatically with derivatives like in the fragment shader. (learn OpenGL)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load heightmap image, create texture and generate mipmaps
    int nChannels;

    unsigned char* data = stbi_load("assets/terrain/Rolling Hills Height Map_32.png", &terrainTexWidth, &terrainTexHeight, &nChannels, 0);
    if (data)
    {
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrainTexWidth, terrainTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        tessShader->setInt("heightMap", 0);
        std::cout << "Loaded terrain heightmap of size " << terrainTexHeight << " x " << terrainTexWidth << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // ----------------------
    // load color map texture
    // ----------------------
    glGenTextures(1, &terrainColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainColorTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters, didn't use mipmap for heightmap because Mip levels won't be selected automatically with derivatives like in the fragment shader. (learn OpenGL)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load color map image, create texture and generate mipmaps
    data = stbi_load("assets/terrain/Rolling Hills Bitmap 1025_32.png", &terrainTexWidth, &terrainTexHeight, &nChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrainTexWidth, terrainTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        tessShader->setInt("colorMap", 1);
        std::cout << "Loaded terrain colormap of size " << terrainTexHeight << " x " << terrainTexWidth << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // vertex generation
    // int width, height;

    terrainTexWidth = terrainTexWidth / 4;
    terrainTexHeight = terrainTexHeight / 4;
    std::vector<float> patchVertices;

    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            patchVertices.push_back(-terrainTexWidth / 2.0f + terrainTexWidth * i / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-terrainTexHeight / 2.0f + terrainTexHeight * j / (float)rez); // v.z
            patchVertices.push_back(i / (float)rez); // u
            patchVertices.push_back(j / (float)rez); // v

            patchVertices.push_back(-terrainTexWidth / 2.0f + terrainTexWidth * (i + 1) / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-terrainTexHeight / 2.0f + terrainTexHeight * j / (float)rez); // v.z
            patchVertices.push_back((i + 1) / (float)rez); // u
            patchVertices.push_back(j / (float)rez); // v

            patchVertices.push_back(-terrainTexWidth / 2.0f + terrainTexWidth * i / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-terrainTexHeight / 2.0f + terrainTexHeight * (j + 1) / (float)rez); // v.z
            patchVertices.push_back(i / (float)rez); // u
            patchVertices.push_back((j + 1) / (float)rez); // v

            patchVertices.push_back(-terrainTexWidth / 2.0f + terrainTexWidth * (i + 1) / (float)rez); // v.x
            patchVertices.push_back(0.0f); // v.y
            patchVertices.push_back(-terrainTexHeight / 2.0f + terrainTexHeight * (j + 1) / (float)rez); // v.z
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
    modelTreeShader = new Shader("shaders/model_tree_vshader.glsl", "shaders/model_tree_fshader.glsl");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // Load car model
    std::string modelPath = "assets/good-dirty-car/car.fbx";
    carModel = new Vehicle("car", glm::vec3(-5.f, 1.f, -5.f), modelPath.data());

    // create plane
    road = new Plane("plane", 5, 300);

    // load tree model
    modelPath = "assets/tree/Gledista_Triacanthos.fbx";
    tree = new Model("tree", glm::vec3(3.f, 0.0, 3.f), modelPath.data());

    // create canvas
    canvas = new Plane("canvas", 2, 2);
    displayDepthMapShader = new Shader("shaders/DisplayDepthMap_vshader.glsl", "shaders/DisplayDepthMap_fshader.glsl");
}

void App::DrawTerrain(Shader* shader)
{
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(terrainVAO);

    shader->setInt("heightMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainHeightTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    shader->setInt("colorMap", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainColorTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    shader->setInt("shadowMap", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object


    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Prepare framebuffer to render depth map for shader mapping
void App::GenerateFrameBuffer()
{
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    // no rendering to color buffer for depthMapFBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::Run()
{
    // render loop
    lastTime = Window::GetTime();
    glEnable(GL_DEPTH_TEST);

    while (!Window::ShouldClose())
    {
        UpdateDeltaTimeAndPhysics();

        // input control camera and vehicle command
        Window::ProcessInput(deltaTime, Physics::getVehicleCommand());

        // start Imgui frame
        Window::StartGUIFrame(&nearPlane, &farPlane, &leftPlane, &rightPlane, &topPlane, &bottomPlane);

        // let the camera follow car
        if (camera->Follow)
        {
            camera->SetFollow(carModel->GetPosition(), carModel->GetRotation());
        }

        RenderDepthMap();

        StartRender();
        // ShowDepthMap();
        RenderScene();

        Window::RenderGUI();
        FinishRender();
    }
}

