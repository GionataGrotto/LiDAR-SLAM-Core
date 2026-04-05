#ifndef UTILS_H
#define UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Enum per gestire i diversi sistemi di coordinate dei sensori
enum AxisMapping {
    XYZ,         // Standard OpenGL (Y=Up, Z=Back)
    XZY_INV,     // Standard LiDAR Anovox (Z=Up, Y=Forward inverted)
    XminusZY     // Altre varianti comuni in robotica
};

namespace Utils {
    // Funzione principale di parsing: legge il file e riempie il vettore outPoints
    // Formato outPoints: [x1, y1, z1, r1, g1, b1, x2, y2, z2, ...]
    bool parsePCD(const std::string& filepath, std::vector<glm::vec3>& outPoints, AxisMapping mode);

    // Funzione per centrare la nuvola di punti (calcola la media e la sottrae)
    void centerPointCloud(std::vector<glm::vec3>& points);

    void processInput(GLFWwindow* window, float deltaTime, Camera& camera, bool& paused);
    
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
}

#endif