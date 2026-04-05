#include "Visualizer.h"
#include <iostream>

Visualizer::Visualizer(): gridVAO(0), gridVBO(0), gridCount(0), pcVAO(0), pcVBO(0), pcCount(0) {}

Visualizer::~Visualizer() {
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &pcVAO);
    glDeleteBuffers(1, &pcVBO);
}

void Visualizer::setupGrid(int size) {
    std::vector<float> gridVertices;
    for (int i = -size; i <= size; i++) {
        float fi = static_cast<float>(i);
        float fs = static_cast<float>(size);
        // Linee Z
        gridVertices.insert(gridVertices.end(), {fi, 0.0f, -fs, fi, 0.0f, fs});
        // Linee X
        gridVertices.insert(gridVertices.end(), {-fs, 0.0f, fi, fs, 0.0f, fi});
    }

    gridCount = (int)gridVertices.size() / 3;

    if (gridVAO == 0) glGenVertexArrays(1, &gridVAO);
    if (gridVBO == 0) glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Visualizer::setPointCloud(const std::vector<glm::vec3>& points) {
    pcCount = static_cast<int>(points.size());
    if (pcCount == 0) return;

    if (pcVAO == 0) glGenVertexArrays(1, &pcVAO);
    if (pcVBO == 0) glGenBuffers(1, &pcVBO);

    glBindVertexArray(pcVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pcVBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Fondamentale: disattiviamo l'array del colore per usare il valore fisso
    glDisableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Visualizer::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("model", glm::mat4(1.0f));

    // 1. Disegna la griglia (Bianca)
    if (gridCount > 0) {
        glBindVertexArray(gridVAO);
        // Imposta il colore fisso per l'attributo 1 (se lo shader lo usa)
        glVertexAttrib3f(1, 0.5f, 0.5f, 0.5f); 
        glDrawArrays(GL_LINES, 0, gridCount);
    }

    // 2. Disegna Point Cloud (Azzurrina)
    if (pcCount > 0) {
        glPointSize(2.0f);
        glBindVertexArray(pcVAO);
        // Colore Azzurrino fisso per tutti i punti
        glVertexAttrib3f(1, 0.2f, 0.6f, 1.0f); 
        glDrawArrays(GL_POINTS, 0, pcCount);
    }

    glBindVertexArray(0);
}

void Visualizer::updateData(const std::vector<glm::vec3>& points) { // <--- Controlla bene i Tipi!
    this->setPointCloud(points);
}