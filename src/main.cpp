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

// --- Configurazione Finestra ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// --- Stato Globale ---
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f)); // Posizionata un po' più in alto e indietro
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Prototipi
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    // 1. Inizializzazione GLFW
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SLAM Engine - Point Cloud View", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 2. Caricamento GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 3. Shader e Visualizer
    Shader ourShader("shaders/default.vert", "shaders/default.frag");
    Visualizer visualizer;

    // Configurazione Griglia
    visualizer.setupGrid(20);
/* 
    // Generazione Nuvola di Punti di Test (5000 punti casuali)
    std::vector<float> points;
    points.reserve(5000 * 6); // 5000 punti * (XYZ + RGB)
    for (int i = 0; i < 5000; i++) {
        // Posizioni casuali (X: -10a10, Y: 0a5, Z: -10a10)
        float x = ((rand() % 2000) / 100.0f) - 10.0f;
        float y = ((rand() % 500) / 100.0f);
        float z = ((rand() % 2000) / 100.0f) - 10.0f;
        
        // Colori basati sulla posizione (Sfumatura ciano/blu)
        float r = 0.0f;
        float g = y / 5.0f; 
        float b = 1.0f;

        points.insert(points.end(), {x, y, z, r, g, b});
    }
    visualizer.setPointCloud(points);
 */
    // Carica il tuo file (assicurati che sia nella cartella corretta)
    if (!visualizer.loadPCD(R"(C:\Users\ggion\Downloads\Anovox_Sample\Anovox\Scenario_f593e8cb-4fe4-4d1a-845f-d6e8020fa9cc\PCD\PCD_6371.pcd)", XZY_INV)) {
        std::cout << "File non trovato o formato non supportato." << std::endl;
    }



    // --- Loop di Rendering ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Sfondo quasi nero
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrici di Trasformazione
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Disegno delegato alla classe Visualizer
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

    // Movimento Camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);

    // Roll (Q ed E)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.Roll -= 100.0f * deltaTime;
        camera.updateCameraVectors();
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.Roll += 100.0f * deltaTime;
        camera.updateCameraVectors();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}