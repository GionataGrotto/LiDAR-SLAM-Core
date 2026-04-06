#include "Odometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <Eigen/Dense>


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

        Eigen::Matrix3f H = Eigen::Matrix3f::Zero();
        
        for (size_t i = 0; i < source_matched.size(); ++i) {
            Eigen::Vector3f s(source_matched[i].x - center_source.x, source_matched[i].y - center_source.y, source_matched[i].z - center_source.z);
            Eigen::Vector3f t(target_matched[i].x - center_target.x, target_matched[i].y - center_target.y, target_matched[i].z - center_target.z);
            
            H += s * t.transpose();
        }

        Eigen::JacobiSVD<Eigen::Matrix3f> svd(H, Eigen::ComputeFullU | Eigen::ComputeThinV);
        Eigen::Matrix3f U = svd.matrixU();
        Eigen::Matrix3f V = svd.matrixV();
        Eigen::Matrix3f R_eigen = V * U.transpose();

        if (R_eigen.determinant() < 0) {
            V.col(2) *= -1;
            R_eigen = V * U.transpose();
        }

        glm::mat4 R_glm =  glm::mat4(1.0f);
        for (size_t x = 0; x < 3; ++x) {
            for (size_t y = 0; y < 3; ++y) {
                R_glm[x][y] = R_eigen(y, x);
            }
        }

        glm::vec3 t_final = center_target - glm::vec3(R_glm * glm::vec4(center_source, 1.0f));

        glm::mat4 T_step = glm::translate(glm::mat4(1.0f), t_final) * R_glm;

        for (auto& p : source_curr) {
            p = glm::vec4(T_step * glm::vec4(p,1.0f));
        }
        
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