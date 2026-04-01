#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    // --- Stato della Camera ---
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // --- Parametri di Orientamento (Angoli di Eulero) ---
    float Yaw;
    float Pitch;
    float Roll;

    // --- Parametri di Navigazione ---
    float MovementSpeed;
    float MouseSensitivity;

    // Costruttore: inizializza i vettori base e gli angoli
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float roll = 0.0f);

    // Funzione core: restituisce la matrice LookAt per lo Shader
    // Deve combinare Posizione, Target (Position + Front) e Up
    inline glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Input Tastiera: aggiorna la Position basandosi sul deltaTime
    // Deve proiettare il movimento lungo i vettori Front o Right
    void ProcessKeyboard(int direction, float deltaTime);

    // Input Mouse: aggiorna Yaw e Pitch basandosi sull'offset del cursore
    // Deve includere il calcolo della sensibilità e il vincolo (clamping) del Pitch
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);

private:
    // Algoritmo di aggiornamento vettoriale:
    // Deve ricalcolare Front, Right e Up ogni volta che gli angoli cambiano
    void updateCameraVectors();
};

#endif