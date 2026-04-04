#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <future>
#include <atomic>
#include "Shader.h"

enum AxisMapping {
    XYZ,        // Standard OpenGL
    XZY_INV,    // Stardard LiDAR (Z=Up, Y=inverted)
    XminusZY    // Common variants
};

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    // Disegna tutto quello che è stato caricato
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    // Carica la griglia
    void setupGrid(int size);

    // Carica una nuvola di punti (XYZ + RGB opzionale)
    void setPointCloud(const std::vector<float>& points);

    // Legge file .pcd
    bool loadPCD(const std::string& filepath, AxisMapping mode = XYZ);

    void loadPCDAsync(const std::string& filepath, AxisMapping mode = XYZ);

    void updateIfNeeded();
private:
    unsigned int gridVAO, gridVBO;
    int gridCount;

    unsigned int pcVAO, pcVBO;
    unsigned int pcCount;

    std::vector<float> threadPoints;
    std::atomic<bool> dataReady{false};
    std::atomic<bool> isFull{false};

    void setupDefaultGrid();
};

#endif