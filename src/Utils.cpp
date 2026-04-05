#include "Utils.h"

namespace Utils {

    bool parsePCD(const std::string& filepath, std::vector<float>& outPoints, AxisMapping mode) {
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

        // 2. Allocazione memoria preventiva (cruciale per le performance)
        outPoints.clear();
        outPoints.reserve(numPoints * 6); // 3 coord + 3 colori

        // 3. Lettura dati binari
        for (int i = 0; i < numPoints; i++) {
            float raw[3];
            file.read(reinterpret_cast<char*>(raw), sizeof(float) * 3);
            
            if (file.gcount() < sizeof(float) * 3) break;

            float fx, fy, fz;
            
            // Remapping degli assi in base al sensore
            switch (mode) {
                case XZY_INV:
                    fx = raw[0];       // X -> X
                    fy = raw[2];       // Z (Altezza LiDAR) -> Y (Up OpenGL)
                    fz = -raw[1];      // Y (Profondità) -> -Z (Forward OpenGL)
                    break;
                case XYZ:
                default:
                    fx = raw[0]; fy = raw[1]; fz = raw[2];
                    break;
            }

            // Inserimento posizione
            outPoints.push_back(fx);
            outPoints.push_back(fy);
            outPoints.push_back(fz);

            // Inserimento colore di default (Azzurro per evidenziare la nuvola)
            outPoints.push_back(0.2f); // R
            outPoints.push_back(0.6f); // G
            outPoints.push_back(1.0f); // B
        }

        file.close();
        return true;
    }

    void centerPointCloud(std::vector<float>& points) {
        if (points.empty()) return;

        double sumX = 0, sumY = 0, sumZ = 0;
        size_t count = points.size() / 6;

        for (size_t i = 0; i < points.size(); i += 6) {
            sumX += points[i];
            sumY += points[i+1];
            sumZ += points[i+2];
        }

        float avgX = static_cast<float>(sumX / count);
        float avgY = static_cast<float>(sumY / count);
        float avgZ = static_cast<float>(sumZ / count);

        for (size_t i = 0; i < points.size(); i += 6) {
            points[i]     -= avgX;
            points[i + 1] -= avgY;
            points[i + 2] -= avgZ;
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

