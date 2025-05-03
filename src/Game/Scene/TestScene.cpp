// src/Game/Scene/TestScene.cpp

#include "TestScene.hpp"
#include "Game/GameObjects/Plane.hpp"
#include "Game/GameObjects/SkyboxGenerator.hpp"
#include "Game/RenderGraphSetup.hpp"
#include "RenderEngine/Config.hpp"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

void TestScene::Setup(ResourceManager* resources, Input* input, RenderEngine* graphics) {
    this->resources = resources;

    camera = FreeCam(1080.0f / 720.0f, 90.0f, 0.1f, 1000.0f);
    camera.setPosition(glm::vec3(0.0f, 1.0f, 1.0f));
    camera.setRotation(glm::radians(glm::vec3(45.0f, 0.0f, 180.0f)));

    input->bindAction("MoveForward", GLFW_KEY_W);
    input->bindAction("MoveBackward", GLFW_KEY_S);
    input->bindAction("MoveLeft", GLFW_KEY_A);
    input->bindAction("MoveRight", GLFW_KEY_D);
    input->bindAction("Sprint", GLFW_KEY_LEFT_SHIFT);
    input->bindAction("ToggleMouseCapture", GLFW_KEY_E);

    globalBuffer = resources->createUniformBuffer(512).value();
    buffers.registerBuffer(&globalBuffer, "Global Buffer");

    Plane* plane = new Plane();
    plane->Setup(resources, &buffers, input);
    gameObjects.push_back(plane);

    start = Duration::now();

    std::shared_ptr<RenderGraph> renderGraph = setupRenderGraph();
    graphics->setRenderGraph(renderGraph);

    // Sky
    skyGenerator.Setup(resources, &buffers, input);

    skybox.Setup(resources, &buffers, input);
    skybox.SetImage(skyGenerator.getImage());
}

void TestScene::Run(Input* input) {
    float time = Duration::since(start).asSeconds();

    ImGui::Begin("Scene Debug");

    glm::vec3 rotation = glm::degrees(glm::eulerAngles(camera.getRotation()));
    glm::vec3 position = camera.getPosition();
    ImGui::Text("Rot: X: %.2f, Y: %.2f, Z: %.2f", rotation.x, rotation.y, rotation.z);
    ImGui::Text("Pos: X: %.2f, Y: %.2f, Z: %.2f", position.x, position.y, position.z);

    ImGui::End();

    if (input->isPressed("ToggleMouseCapture") && input->isChanged("ToggleMouseCapture"))
        input->setCapture(!input->isCapturing());

    glm::vec2 move(0.0f);
    if (input->isPressed("MoveLeft"))       move.x--;
    if (input->isPressed("MoveRight"))      move.x++;
    if (input->isPressed("MoveBackward"))   move.y--;
    if (input->isPressed("MoveForward"))    move.y++;
    if (input->isPressed("Sprint"))         move *= 2.0f;

    if (input->isCapturing()) {
        camera.move(move, input->deltaTime().asSeconds());
        camera.mouseDelta(input->mouseDelta());
    }

    // Update Global Buffer
    U8* globalPtr = reinterpret_cast<uint8_t*>(globalBuffer.info.pMappedData);

    struct {
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec3 cameraPosition;
        float time;
    } global;

    global.view = camera.getViewMatrix();
    global.proj = camera.getProjectionMatrix();
    global.cameraPosition = camera.getPosition();
    global.time = time;

    memcpy(globalPtr, &global, sizeof(global));

    struct {
        glm::vec3 dirLightDir;
        float _pad1;
        glm::vec3 dirLightColor;
        float _pad2;
        struct {
            glm::vec3 pos;
            float _pad1;
            glm::vec3 color;
            float intensity;
        } pointLights[4];
        int numPointLights;
        int _pad3[3];
    } lights;

    lights.dirLightDir = glm::normalize(glm::vec3(0.5f, -1.0f, 0.2f));
    lights.dirLightColor = glm::vec3(1.0f);
    lights.pointLights[0] = {
        glm::vec3(2, 2, 2),
        0.0f,
        glm::vec3(1.0f),
        20.0f
    };
    lights.pointLights[1] = {
        glm::vec3(-2, 1, -2),
        0.0f,
        glm::vec3(1.0f),
        15.0f
    };
    lights.numPointLights = 2;

    memcpy(globalPtr + 192, &lights, sizeof(lights));

    for (Size i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Run(input);
    }

    skyGenerator.Run(input);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 viewNoTranslation = view;
    viewNoTranslation[3] = glm::vec4(0, 0, 0, 1);
    glm::mat4 viewProj = camera.getProjectionMatrix() * viewNoTranslation;
    skybox.SetViewProj(viewProj);
    skybox.Run(input);
}

void TestScene::Draw(RenderEngine* graphics) {
    skyGenerator.Draw(graphics);
    skybox.Draw(graphics);

    for (Size i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Draw(graphics);
    }
}

void TestScene::Cleanup() {
    skyGenerator.Cleanup(resources);
    skybox.Cleanup(resources);

    globalBuffer.shutdown();
    for (Size i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Cleanup(resources);
        delete gameObjects[i];
    }
}

