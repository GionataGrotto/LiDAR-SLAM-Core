#include "Utils.h"

namespace Utils {

    bool parsePCD(const std::string& filepath, std::vector<glm::vec3>& outPoints, AxisMapping mode) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[Utils] Errore: impossibile aprire " << filepath << std::endl;
            return false;
        }

        std::string line;
        int numPoints = 0;
        bool isBinary = false;

        // 1. Lettura Header (testuale)
        while (std::getline(file, line)) {
            if (line.find("POINTS") == 0) {
                numPoints = std::stoi(line.substr(7));
            }
            if (line.find("DATA binary") == 0) {
                isBinary = true;
                break; // I dati iniziano subito dopo questa riga
            }
        }

        if (!isBinary || numPoints <= 0) {
            std::cerr << "[Utils] Errore: formato non supportato o file vuoto." << std::endl;
            return false;
        }

        // Pre-allochiamo lo spazio per i vettori
        outPoints.resize(numPoints); 

        // Leggiamo TUTTI i punti in un colpo solo! 
        // sizeof(glm::vec3) è esattamente 12 byte (3 * 4 byte del float)
        file.read(reinterpret_cast<char*>(outPoints.data()), numPoints * sizeof(glm::vec3));

        // Se dobbiamo invertire gli assi (es. XZY_INV), lo facciamo "in place"
        if (mode == XZY_INV) {
            for (auto& p : outPoints) {
                float oldY = p.y;
                float oldZ = p.z;
                p.y = oldZ;    // Z diventa Y (Up)
                p.z = -oldY;   // Y diventa -Z (Forward)
            }
        }

        file.close();
        return true;
    }

    void centerPointCloud(std::vector<glm::vec3>& points) {
        if (points.empty()) return;

        glm::vec3 sum(0.0);
        for (const auto& p : points) {
            sum += p;
        }

        glm::vec3 center = sum / static_cast<float>(points.size());

        for (auto& p : points) {
            p -= center;
        }
    }

    void processInput(GLFWwindow* window, float deltaTime, Camera& camera, bool& paused) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Spazio per mettere in pausa/play
        static bool spacePressed = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            paused = !paused;
            spacePressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressed = false;

        // Movimento Camera (usiamo i parametri passati alla funzione)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
}

