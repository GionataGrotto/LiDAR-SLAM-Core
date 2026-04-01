#version 330 core
layout (location = 0) in vec3 aPos;   // La posizione del vertice
layout (location = 1) in vec3 aColor; // Il colore del vertice (opzionale)

out vec3 ourColor;

// Matrici di trasformazione
uniform mat4 model;      // Trasformazione dell'oggetto (posizione nel mondo)
uniform mat4 view;       // Trasformazione della Camera (la tua GetViewMatrix)
uniform mat4 projection; // Trasformazione di Prospettiva (FOV, aspect ratio)

void main() {
    // Calcolo della posizione finale: Proiezione * Vista * Modello * Punto
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}