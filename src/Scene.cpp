#include "Scene.h"
#include <thread>

void Scene::addFrame(const std::string& filepath, AxisMapping mode) {
    std::thread([this, filepath, mode]() {
        std::vector<glm::vec3> localPoints;

        if (Utils::parsePCD(filepath, localPoints, mode)) {
            Utils::centerPointCloud(localPoints);

            std::lock_guard<std::mutex> lock(dataMutex);
            currentPoints = std::move(localPoints);
            newDataAvailable = true;
        }
    }).detach();
}

const std::vector<glm::vec3>& Scene::getPointsToRender() {
    std::lock_guard<std::mutex> lock(dataMutex);
    newDataAvailable = false;
    return currentPoints;
}