#include "Odometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>


glm::mat4 Odometry::computeICP(const std::vector<glm::vec3>& source, 
                         const std::vector<glm::vec3>& target, 
                         unsigned int iterations) {

    glm::mat4 T_total = glm::mat4(1.0f);
    
    std::vector<glm::vec3> source_curr(source);

    glm::vec3 centerSource = getCenterOfMass(source);

    for (unsigned int iter = 0; iter < iterations; ++iter) {
        std::vector<glm::vec3> target_matched;
        std::vector<glm::vec3> source_matched;

        for (size_t i = 0; i < source_curr.size(); i+=100) {
            glm::vec3 p_src = source_curr[i];

            float min_dist = std::numeric_limits<float>::max();
            glm::vec3 closest_pt;

            for (size_t j = 0; j < target.size(); j+=10) {
                glm::vec3 p_tgt = target[j];

                glm::vec3 diff = p_src - p_tgt;
                float d = glm::dot(diff, diff);
                
                if (d < min_dist) {
                    min_dist = d;
                    closest_pt = p_tgt;
                }
            }
            source_matched.push_back(p_src);
            target_matched.push_back(closest_pt);
        }

        glm::vec3 center_source = getCenterOfMass(source_matched);
        glm::vec3 center_target = getCenterOfMass(target_matched);

        glm::vec3 t = center_target - center_source;
        
        glm::mat4 T_step = glm::translate(glm::mat4(1.0f), t);

        for (auto& p : source_curr) {
            p = glm::vec3(T_step * glm::vec4(p, 1.0f));
        }
        T_total = T_step * T_total;       
        
    }

    return T_total;
}


glm::vec3 Odometry::getCenterOfMass(const std::vector<glm::vec3>& points) {
    glm::vec3 sum{0.0f,0.0f,0.0f};
    for (const auto &p : points) {
        sum += p;
    }
    return sum / static_cast<float>(points.size());
}