#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Visualizer.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm> // Per std::sort

namespace fs = std::filesystem;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 5.0f, 20.0f)); 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Gestione Sequenza
std::vector<std::string> pcdFiles;
int currentFrameIndex = 0;
float frameTimer = 0.0f;
float frameDuration = 0.1f; // 10 FPS (cambia a 0.5 per andare più lento)
bool paused = false;

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SLAM Engine - Sequence Player", NULL, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shaders/default.vert", "shaders/default.frag");
    Visualizer visualizer;
    visualizer.setupGrid(50);

    // 1. Raccogliamo e ordiniamo i file PCD
    std::string path = R"(C:\Users\ggion\Downloads\Anovox_Sample\Anovox\Scenario_f593e8cb-4fe4-4d1a-845f-d6e8020fa9cc\PCD)";
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".pcd") {
            pcdFiles.push_back(entry.path().string());
        }
    }
    std::sort(pcdFiles.begin(), pcdFiles.end()); // Fondamentale per l'ordine cronologico

    if (pcdFiles.empty()) {
        std::cout << "Nessun file PCD trovato!" << std::endl;
        return -1;
    }

    // Carichiamo il primo frame
    visualizer.loadPCD(pcdFiles[currentFrameIndex], XZY_INV);

    // --- Loop di Rendering ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        visualizer.updateIfNeeded();

        // 2. Logica di avanzamento automatico dei frame
        if (!paused) {
            frameTimer += deltaTime;
            if (frameTimer >= frameDuration) {
                frameTimer = 0.0f;
                currentFrameIndex = (currentFrameIndex + 1) % pcdFiles.size();
                visualizer.loadPCDAsync(pcdFiles[currentFrameIndex], XZY_INV);
                
            }
        }

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

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Spazio per mettere in pausa/play
    static bool spacePressed = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        paused = !paused;
        spacePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressed = false;

    // Movimento standard
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}