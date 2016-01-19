// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "IdDistanceTupel.h"

IdDistanceTupel::IdDistanceTupel(int closestPointId, double distance) {
    this->closestPointId = closestPointId;
    this->distance = distance;
}

int IdDistanceTupel::getClosestPointId() {
    return closestPointId;
}

double IdDistanceTupel::getDistance() {
    return distance;
}