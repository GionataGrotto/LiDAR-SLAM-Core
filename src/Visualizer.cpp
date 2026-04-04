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

bool Visualizer::loadPCD(const std::string& filepath, AxisMapping mode) {
    std::ifstream file(filepath, std::ios::binary);

    if (!file.is_open()) return false;

    std::string line;
    int numPoints = 0;
    bool isBinary = false;

    // 1. Leggi l'Header (formato testo)
    while (std::getline(file, line)) {
        if (line.find("POINTS") == 0) {
            numPoints = std::stoi(line.substr(7));
        }
        if (line.find("DATA binary") == 0) {
            isBinary = true;
            break; // Dopo "DATA binary" iniziano i byte
        }
    }

    if (!isBinary) {
        std::cerr << "Errore: Il file non è in formato binary." << std::endl;
        return false;
    }

    // 2. Lettura dei dati binari
    std::vector<float> points;
    points.reserve(numPoints * 6); // XYZ + RGB

    for (int i = 0; i < numPoints; i++) {
        float raw_coords[3];
        // Leggiamo 12 byte (3 float x 4 byte)
        file.read(reinterpret_cast<char*>(raw_coords), sizeof(float) * 3);

        if (file.gcount() < sizeof(float) * 3) break; // Fine file inaspettata
        
        float finalX, finalY, finalZ;
        switch(mode) {
            case XZY_INV: 
                finalX = raw_coords[0];  // X -> X
                finalY = raw_coords[2];  // Z -> Y (Up)
                finalZ = -raw_coords[1]; // Y -> Z (Forward)
                break;
            case XYZ:
            default:
                finalX = raw_coords[0]; finalY = raw_coords[1]; finalZ = raw_coords[2];
                break;
        }

        points.push_back(finalX); points.push_back(finalY); points.push_back(finalZ);

        // Aggiungiamo un colore (es. Bianco/Azzurro) perché il nostro shader vuole RGB
        points.push_back(0.3f); // R
        points.push_back(0.7f); // G
        points.push_back(1.0f); // B
    }

    file.close();

    if (!points.empty()) {
        // Compute mean for every XYZ
        float avgX = 0;
        float avgY = 0;
        float avgZ = 0;
        for (size_t i = 0; i < points.size(); i+=6) {
            avgX += points[i];
            avgY += points[i+1];
            avgZ += points[i+2];
        }
        avgX /= numPoints; 
        avgY /= numPoints; 
        avgZ /= numPoints;

        // Subtract the mean from each point to center the pointcloud
        for(size_t i = 0; i < points.size(); i+=6) {
            points[i] -= avgX; 
            points[i+1] -= avgY; 
            points[i+2] -= avgZ;
        }


        this->setPointCloud(points);
        std::cout << "Successo! Caricati " << numPoints << " punti binari." << std::endl;
        return true;
    }

    return false;
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

#include <thread>

void Visualizer::loadPCDAsync(const std::string& filepath, AxisMapping mode) {
    // Se il thread precedente non ha ancora finito o il thread principale 
    // non ha ancora prelevato i dati, evitiamo di sovrascrivere o accumulare thread.
    if (this->dataReady) {
        return;
    }

    // Lanciamo un thread "fire-and-forget"
    std::thread([this, filepath, mode]() {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Errore: Impossibile aprire " << filepath << std::endl;
            return;
        }

        std::string line;
        int numPoints = 0;
        bool isBinary = false;

        // 1. Parsing dell'Header (formato testo)
        while (std::getline(file, line)) {
            if (line.find("POINTS") == 0) {
                numPoints = std::stoi(line.substr(7));
            }
            if (line.find("DATA binary") == 0) {
                isBinary = true;
                break; 
            }
        }

        if (!isBinary || numPoints <= 0) {
            return;
        }

        // 2. Lettura dei dati binari
        std::vector<float> localPoints;
        localPoints.reserve(numPoints * 6); // XYZ + RGB

        for (int i = 0; i < numPoints; i++) {
            float raw_coords[3];
            file.read(reinterpret_cast<char*>(raw_coords), sizeof(float) * 3);

            if (file.gcount() < sizeof(float) * 3) break;
            
            float finalX, finalY, finalZ;
            switch(mode) {
                case XZY_INV: 
                    finalX = raw_coords[0];  // X -> X
                    finalY = raw_coords[2];  // Z -> Y (Up in OpenGL)
                    finalZ = -raw_coords[1]; // Y -> Z (Forward in OpenGL)
                    break;
                case XYZ:
                default:
                    finalX = raw_coords[0]; 
                    finalY = raw_coords[1]; 
                    finalZ = raw_coords[2];
                    break;
            }

            // Inserimento coordinate
            localPoints.push_back(finalX); 
            localPoints.push_back(finalY); 
            localPoints.push_back(finalZ);

            // Inserimento Colore (Azzurro chiaro predefinito)
            localPoints.push_back(0.3f); // R
            localPoints.push_back(0.7f); // G
            localPoints.push_back(1.0f); // B
        }
        file.close();

        // 3. Auto-centramento (Media Mobile)
        if (!localPoints.empty()) {
            double sumX = 0, sumY = 0, sumZ = 0;
            size_t actualPoints = localPoints.size() / 6;

            for (size_t i = 0; i < localPoints.size(); i += 6) {
                sumX += localPoints[i];
                sumY += localPoints[i+1];
                sumZ += localPoints[i+2];
            }
            
            float avgX = (float)(sumX / actualPoints);
            float avgY = (float)(sumY / actualPoints);
            float avgZ = (float)(sumZ / actualPoints);

            for (size_t i = 0; i < localPoints.size(); i += 6) {
                localPoints[i]   -= avgX; 
                localPoints[i+1] -= avgY; 
                localPoints[i+2] -= avgZ;
            }

            // Spostiamo i dati nel buffer della classe e segnaliamo che siamo pronti
            // Nota: threadPoints deve essere una variabile membro di Visualizer
            this->threadPoints = std::move(localPoints);
            this->dataReady = true; // Variabile atomica: segnala al thread principale di aggiornare
        }
    }).detach(); // Il thread corre in background e si autodistrugge al termine
}

void Visualizer::updateIfNeeded() {
    if (dataReady) {
        // Ora siamo nel thread principale, possiamo chiamare funzioni OpenGL
        setPointCloud(threadPoints);
        threadPoints.clear();
        dataReady = false;
    }
}