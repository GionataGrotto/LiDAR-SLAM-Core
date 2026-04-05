#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include "Shader.h"
#include "Visualizer.h"
#include "Scene.h"
#include "Odometry.h"
#include "Camera.h"

namespace fs = std::filesystem;

// Callback per il ridimensionamento della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // 1. Inizializzazione GLFW
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Anovox LiDAR SLAM", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 2. Inizializzazione GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);

    // 3. Setup Cartella PCD e Scansione File
    std::string pcdPath = R"(C:\Users\ggion\Downloads\Anovox_Sample\Anovox\Scenario_f593e8cb-4fe4-4d1a-845f-d6e8020fa9cc\PCD)";
    std::vector<std::string> pcdFiles;

    try {
        for (const auto& entry : fs::directory_iterator(pcdPath)) {
            if (entry.path().extension() == ".pcd") {
                pcdFiles.push_back(entry.path().string());
            }
        }
        // Ordiniamo i file per nome (fondamentale per la sequenza temporale)
        std::sort(pcdFiles.begin(), pcdFiles.end());
    } catch (const std::exception& e) {
        std::cerr << "Errore accesso cartella: " << e.what() << std::endl;
        return -1;
    }

    if (pcdFiles.empty()) {
        std::cerr << "Nessun file .pcd trovato nella cartella!" << std::endl;
        return -1;
    }

    // 4. Setup Componenti SLAM
    Shader shader("shaders/default.vert", "shaders/default.frag");
    Visualizer visualizer;
    visualizer.setupGrid(50);
    
    Scene scene;
    Odometry odometry;
    Camera camera(glm::vec3(0.0f, 20.0f, 50.0f));

    std::vector<glm::vec3> lastFrame;
    glm::mat4 currentAutoPose = glm::mat4(1.0f);
    size_t currentFileIdx = 0;

    // Carichiamo il primo frame
    scene.addFrame(pcdFiles[currentFileIdx], XZY_INV);

    // 5. Loop Principale
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // LOGICA SLAM: Quando Scene ha finito di caricare un frame
        if (scene.hasNewData()) {
            const std::vector<glm::vec3>& currentFrame = scene.getPointsToRender();

            if (!lastFrame.empty()) {
                // Eseguiamo ICP (5 iterazioni per velocità)
                glm::mat4 deltaPose = odometry.computeICP(lastFrame, currentFrame, 5);
                
                // Aggiorniamo la posizione cumulativa
                currentAutoPose = currentAutoPose * deltaPose;

                // Debug in console
                std::cout << "Elaborato: " << pcdFiles[currentFileIdx] << std::endl;
                std::cout << "Auto Pos Z: " << currentAutoPose[3][2] << std::endl;
            }

            // Aggiorniamo la GPU
            visualizer.updateData(currentFrame);
            
            // Prepariamo per il prossimo
            lastFrame = currentFrame;

            // Carichiamo il file successivo se disponibile
            currentFileIdx++;
            if (currentFileIdx < pcdFiles.size()) {
                scene.addFrame(pcdFiles[currentFileIdx], XZY_INV);
            } else {
                std::cout << "--- Sequenza Completata ---" << std::endl;
            }
        }

        // 6. Rendering
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        visualizer.draw(shader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}