#ifndef KDTREE_H
#define KDTREE_H

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <limits>

struct KDNode {
    glm::vec3 point;
    int index;

    KDNode *left = nullptr;
    KDNode *right = nullptr;

    // RIMOSSO il ; dopo la parentesi tonda e aggiunto le parentesi graffe
    KDNode(glm::vec3 p, int i) : point(p), index(i), left(nullptr), right(nullptr) {}

    ~KDNode() {
        if (left) delete left;
        if (right) delete right;
    } // RIMOSSO il ; dopo la graffa
};

class KDTree {
public:
    KDTree(const std::vector<glm::vec3>& points);
    // È buona norma aggiungere il distruttore per pulire la root
    ~KDTree();

    int nearestNeighbor(glm::vec3 query) const;

private:
    KDNode* root = nullptr;
    const std::vector<glm::vec3>& pointsRef;

    KDNode* buildRecursive(std::vector<int>& indices, int start, int end, int depth);
    void searchRecursive(KDNode* node, glm::vec3 query, int depth, float& bestDistSq, int& bestIndex) const;
};

#endif