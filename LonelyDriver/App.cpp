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
#include "Skybox.h"
#include "Light.h"
#include "Plane.h"


void PrintVec3(glm::vec3 v)
{
    std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}


App::App(const int width, const int height, const std::string& title) :
    SCR_WIDTH(width), SCR_HEIGHT(height), TITLE(title), deltaTime(1.0f / 60.0f), lastFrame(0.0f),
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

// Set camera view and projection tranformations
void App::StartRender()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = camera->GetViewMatrix();
}

// Check all events and swap front and back buffers, update delta time for frame-dependent physics
void App::FinishRender()
{
    if (lastFrame != 0.0f) // First deltaTime is initialized to 1.0f / 60.0f;
    {
        deltaTime = Window::GetTime() - lastFrame;
    }

    Window::PollEvents();
    Window::SwapBuffers();

    lastFrame = Window::GetTime();

    if (oneSec >= 1.0f)
    {
        std::clog << "\r" << frameCount / oneSec << " FPS" << std::flush;
        frameCount = 0;
        oneSec = 0.f;
    }
    else
    {
        oneSec += deltaTime;
        frameCount++;
    }
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
    // load height map texture
    int width, height, nChannels;
    unsigned char* data = stbi_load("assets/heightmap.png", &width, &height, &nChannels, 0);

    // vertex generation
    std::vector<float> vertices;
    float yScale = 64.0f / 256.0f, yShift = 0.0f;  // apply a scale+shift to the height data
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            // vertex
            vertices.push_back(-height / 2.0f + i);       // v.x
            vertices.push_back((int)y * yScale - yShift); // v.y
            vertices.push_back(-width / 2.0f + j);        // v.z
        }
    }

    stbi_image_free(data);

    // index generation
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < height - 1; i++)       // for each row a.k.a. each strip
    {
        for (unsigned int j = 0; j < width; j++)      // for each column
        {
            for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    NUM_STRIPS = height - 1;
    NUM_VERTS_PER_STRIP = width * 2;

    // register VAO
    // GLuint terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),       // size of vertices buffer
        &vertices[0],                          // pointer to first element
        GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int), // size of indices buffer
        &indices[0],                           // pointer to first element
        GL_STATIC_DRAW);

}

void App::CreateDrawableObjects()
{
    // load shaders
    lightShader = new Shader("shaders/lightSource_vshader.glsl", "shaders/lightSource_fshader.glsl");
    modelShader = new Shader("shaders/model_load_vshader.glsl", "shaders/model_load_fshader.glsl");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Load car model
    std::string modelPath = "assets/audi/audi.obj";
    carModel = new Model("car", glm::vec3(0.f, 1.f, 0.f), modelPath.data());

    // create light cube
    lightCube = new Light("light", glm::vec3(3.f, 2.f, 0.f));

    // create plane
    road = new Plane("plane");
}

void App::Run()
{
    // render loop
    glEnable(GL_DEPTH_TEST);
    while (!Window::ShouldClose())
    {
                // input
        Window::ProcessInput(deltaTime);

        // calculate physics, update object world poses (position and rotation) and light position
        Physics::Step(deltaTime, objectGlobalPoses, lightCube);

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

        DrawWireframe = false;
        // render plane
        // road->Draw(*modelShader, DrawWireframe);

        // render the car
        model = glm::mat4(1.0f);
        model = model * objectGlobalPoses[carModel->Name];
        carModel->Draw(*modelShader, model, DrawWireframe);

        // render light source
        lightShader->use();
        lightShader->setMat4("model", objectGlobalPoses[lightCube->Name]);
        lightShader->setMat4("view", view);
        lightShader->setMat4("projection", projection);

        lightCube->Draw(*lightShader, DrawWireframe);
        // PrintVec3(lightCube->Position);
        
        // render terrain
        model = glm::mat4(1.0f);
        lightShader->setMat4("model", model);
        glBindVertexArray(terrainVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // render the mesh triangle strip by triangle strip - each row at a time
        for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                NUM_VERTS_PER_STRIP, // number of indices to render
                GL_UNSIGNED_INT,     // index data type
                (void*)(sizeof(unsigned int)
                    * NUM_VERTS_PER_STRIP
                    * strip)); // offset to starting index
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // render skybox
        skyboxShader->use();
        view = glm::mat4(glm::mat3(view)); // remove translation section, only keep rotation section of camera
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        skybox->Draw(*skyboxShader, DrawWireframe);

        FinishRender();
    }
}

