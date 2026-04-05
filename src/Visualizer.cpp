#include "Visualizer.h"
#include <iostream>

Visualizer::Visualizer(): gridVAO(0), gridVBO(0), gridCount(0), pcVAO(0), pcVBO(0), pcCount(0)
{

}

Visualizer::~Visualizer() {
    // Pulizia della memoria video quando l'oggetto viene distrutto
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &pcVAO);
    glDeleteBuffers(1, &pcVBO);
}

void Visualizer::setupGrid(int size) {
    constexpr int COMPONENTS_PER_VERTEX = 3; // x, y, z
    constexpr int VERTICES_PER_LINE = 2; // start, end
    constexpr int LINES_PER_ITERATION = 2; // lungo Z, lungo X
    constexpr int STRIDE = COMPONENTS_PER_VERTEX * VERTICES_PER_LINE * LINES_PER_ITERATION;

    std::vector<float> gridVertices((size *2 + 1) * STRIDE);

    for (int i = -size; i <= size; i++) {
        const int base = (i + size) * STRIDE;
        const float fi = static_cast<float>(i);
        const float fs = static_cast<float>(size);

        // Linee lungo Z
        // Start
        gridVertices[base + 0] = fi;
        gridVertices[base + 1] = 0.0f;
        gridVertices[base + 2] = -fs;
        // End
        gridVertices[base + 3] = fi;
        gridVertices[base + 4] = 0.0f;
        gridVertices[base + 5] = fs;

        // Linee lungo X
        // Start
        gridVertices[base + 6] = -fs;
        gridVertices[base + 7] = 0.0f;
        gridVertices[base + 8] = fi;

        // End
        gridVertices[base + 9] = fs;
        gridVertices[base + 10] = 0.0f;
        gridVertices[base + 11] = fi;
    }

    gridCount = (int)gridVertices.size() / 3;

    if (gridVAO == 0) glGenVertexArrays(1, &gridVAO);
    if (gridVBO == 0) glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    // Spieghiamo a OpenGL che sono solo Posizioni (X, Y, Z) -> Location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Visualizer::setPointCloud(const std::vector<float>& points) {
    pcCount = static_cast<int>(points.size() / 6);

    if (pcVAO == 0) {
        glGenVertexArrays(1, &pcVAO);
    }

    if (pcVBO == 0) {
        glGenBuffers(1, &pcVBO);
    }

    glBindVertexArray(pcVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pcVBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_DYNAMIC_DRAW);

    // Attributo 0: Posizione (XYZ) - primi 3 float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attributo 1. Colore (RGB) - successivi 3 float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Visualizer::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // Disegna la griglia se esiste
    if (gridCount > 0) {
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridCount);
    }

    // Disegno la point cloud se esiste
    if (pcCount > 0) {
        glPointSize(2.0f);
        glBindVertexArray(pcVAO);
        glDrawArrays(GL_POINTS, 0, pcCount);
    }

    glBindVertexArray(0);
}


void Visualizer::updateData(const std::vector<float>& points) {
    this->setPointCloud(points);
}