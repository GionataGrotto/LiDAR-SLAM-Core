#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Utils.h"
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

class Scene {
public:
    Scene() : newDataAvailable(false) {}

    void addFrame(const std::string& filepath, AxisMapping mode);

    bool hasNewData() const { return newDataAvailable; }
    
    const std::vector<glm::vec3>& getPointsToRender();

private:
    std::vector<glm::vec3> currentPoints;
    std::atomic<bool> newDataAvailable;
    std::mutex dataMutex;
};

#endif
