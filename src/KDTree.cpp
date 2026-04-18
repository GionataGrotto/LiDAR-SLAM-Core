#include "KDTree.h"
#include <limits>
#include <numeric>
#include <algorithm>

KDTree::KDTree(const std::vector<glm::vec3>& points) : pointsRef(points) {
    if (points.empty()) return;
    std::vector<int> indices(points.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Passiamo 0 come inizio e size come fine
    root = buildRecursive(indices, 0, (int)indices.size(), 0);
}

KDTree::~KDTree() {
    delete root;
}

KDNode* KDTree::buildRecursive(std::vector<int>& indices, int start, int end, int depth) {
    if (start >= end) return nullptr; // Base della ricorsione

    int axis = depth % 3;
    int mid = start + (end - start) / 2;

    // Ordina solo la porzione che ci interessa [start, end)
    std::nth_element(indices.begin() + start, indices.begin() + mid, indices.begin() + end,
        [&](int a, int b) {
            return pointsRef[a][axis] < pointsRef[b][axis];
        });

    KDNode* node = new KDNode(pointsRef[indices[mid]], indices[mid]);

    // RICORSIONE: Usiamo gli indici invece di creare nuovi vector
    node->left = buildRecursive(indices, start, mid, depth + 1);
    node->right = buildRecursive(indices, mid + 1, end, depth + 1);

    return node;
}

int KDTree::nearestNeighbor(glm::vec3 query) const {
    float bestDistSq = std::numeric_limits<float>::max();
    int bestIndex = -1;
    searchRecursive(root, query, 0, bestDistSq, bestIndex);
    return bestIndex;
}

void KDTree::searchRecursive(KDNode* node, glm::vec3 query, int depth, float& bestDistSq, int& bestIndex) const {
    if (!node) return;

    // Calcoliamo la distanza al quadrato (più veloce, evita la radice quadrata)
    glm::vec3 diff = query - node->point;
    float distSq = glm::dot(diff, diff);

    if (distSq < bestDistSq) {
        bestDistSq = distSq;
        bestIndex = node->index;
    }

    int axis = depth % 3;
    float planeDiff = query[axis] - node->point[axis];

    KDNode* nearSide = (planeDiff < 0) ? node->left : node->right;
    KDNode* farSide = (planeDiff < 0) ? node->right : node->left;

    // Esplora il lato più promettente
    searchRecursive(nearSide, query, depth + 1, bestDistSq, bestIndex);

    // Esplora l'altro lato solo se c'è possibilità di trovare un punto più vicino
    if (planeDiff * planeDiff < bestDistSq) {
        searchRecursive(farSide, query, depth + 1, bestDistSq, bestIndex);
    }
}