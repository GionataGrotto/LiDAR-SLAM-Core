cmake_minimum_required(VERSION 3.15)
project(SLAM_Project LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Trova le dipendenze tramite vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# 2. Specifica dove sono gli header (.h)
include_directories(include)

# 3. Definisci i file sorgente
set(SOURCES 
    src/main.cpp 
    src/Camera.cpp 
    src/Shader.cpp
    src/Visualizer.cpp
    src/Utils.cpp
    src/Scene.cpp
    src/Odometry.cpp
)

# 4. Crea l'eseguibile
add_executable(${PROJECT_NAME} ${SOURCES})

# 5. Collega le librerie
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glm::glm
    glad::glad
)

# 6. COPIA AUTOMATICA DEGLI SHADER (Fondamentale!)
# Questo copia la cartella 'shaders' dentro 'build' ogni volta che compili
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})