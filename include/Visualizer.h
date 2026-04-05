#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <future>
#include <atomic>
#include "Shader.h"

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    // Disegna tutto quello che è stato caricato
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    // Carica la griglia
    void setupGrid(int size);

    // Carica una nuvola di punti (XYZ + RGB opzionale)
    void setPointCloud(const std::vector<glm::vec3>& points);

    void updateData(const std::vector<glm::vec3>& points);
    
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