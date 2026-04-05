#ifndef ODOMETRY_H
#define ODOMETRY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Odometry {
public:
    // La funzione core: riceve il frame precedente e quello attuale
    // Restituisce la trasformazione relativa (R e T)
    glm::mat4 computeICP(const std::vector<glm::vec3>& source, 
                         const std::vector<glm::vec3>& target, 
                         unsigned int iterations = 10);

private:
    // Funzioni di supporto matematico (SVD, Nearest Neighbor, ecc.)
    glm::vec3 getCenterOfMass(const std::vector<glm::vec3>& points);
};

#endif