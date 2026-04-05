#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Visualizer.h"
#include "Scene.h"
#include "Utils.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

// --- Configurazione ---
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// --- Stato Globale ---
Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool paused = false;

// Dati della sequenza
std::vector<std::string> pcdFiles;
int currentFrameIndex = 0;
float frameTimer = 0.0f;
float frameDuration = 0.1f; // 10 FPS

int main() {
    // 1. Inizializzazione Finestra (GLFW + GLAD)
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gemini SLAM Engine", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Utils::framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);

    // 2. Inizializzazione Componenti Core
    Shader ourShader("shaders/default.vert", "shaders/default.frag");
    Visualizer visualizer;
    Scene scene; // Il nuovo cervello dei dati
    
    visualizer.setupGrid(50);

    // 3. Scansione Dataset
    std::string path = R"(C:\Users\ggion\Downloads\Anovox_Sample\Anovox\Scenario_f593e8cb-4fe4-4d1a-845f-d6e8020fa9cc\PCD)";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".pcd") 
            pcdFiles.push_back(entry.path().string());
    }
    std::sort(pcdFiles.begin(), pcdFiles.end());

    if (pcdFiles.empty()) {
        std::cerr << "Dataset non trovato!" << std::endl;
        return -1;
    }

    // Carichiamo il primo frame immediatamente
    scene.addFrame(pcdFiles[currentFrameIndex], XZY_INV);

    // --- Loop di Rendering ---
    while (!glfwWindowShouldClose(window)) {
        // A. Gestione Tempo
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // B. Input (Ora gestito in Utils)
        Utils::processInput(window, deltaTime, camera, paused);

        // C. Sincronizzazione Scena -> Visualizer
        // Se la Scene ha finito di caricare i punti nel thread, li passiamo alla GPU
        if (scene.hasNewData()) {
            visualizer.updateData(scene.getPointsToRender());
        }

        // D. Logica di Avanzamento
        if (!paused) {
            frameTimer += deltaTime;
            if (frameTimer >= frameDuration) {
                frameTimer = 0.0f;
                currentFrameIndex = (currentFrameIndex + 1) % pcdFiles.size();
                
                // Chiamata asincrona alla scena
                scene.addFrame(pcdFiles[currentFrameIndex], XZY_INV);
            }
        }

        // E. Rendering
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        visualizer.draw(ourShader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}