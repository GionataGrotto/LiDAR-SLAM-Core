# 1. Deve essere SEMPRE la prima riga
cmake_minimum_required(VERSION 3.15)

# 2. Deve essere la seconda riga (importante per gli avvisi che ricevi)
project(SLAM_Project LANGUAGES CXX C)

# 3. Imposta lo standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Cerca i pacchetti installati tramite vcpkg
# Se il settings.json è corretto, vcpkg scaricherà glfw, glm e glad automaticamente
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# 5. Aggiungi la cartella degli header
include_directories(include)

# 6. Crea l'eseguibile (assicurati che i file esistano in queste cartelle)
add_executable(${PROJECT_NAME} 
    src/main.cpp 
    src/Camera.cpp
)

# 7. Collega le librerie
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glm::glm
    glad::glad
)