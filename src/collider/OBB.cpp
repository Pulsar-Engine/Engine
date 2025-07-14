#include "OBB.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "MeshCollider.hpp"

OBB::OBB(const glm::vec3 &center, const glm::vec3 &halfSize, const glm::mat3 &rotationMatrix)
    : _center(center), _halfSize(halfSize), _rotationMatrix(rotationMatrix) {}

IntersectionResult OBB::intersect(const AABB &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult OBB::intersect(const Sphere &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult OBB::intersect(const OBB &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 axesA[3] = {_rotationMatrix[0], _rotationMatrix[1], _rotationMatrix[2]};
    glm::vec3 axesB[3] = {other._rotationMatrix[0], other._rotationMatrix[1], other._rotationMatrix[2]};
    glm::vec3 centerDiff = other._center - _center;
    float minOverlap = FLT_MAX;
    glm::vec3 minOverlapAxis(0.0f);
    float R[3][3];
    float AbsR[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = glm::dot(axesA[i], axesB[j]);
            AbsR[i][j] = glm::abs(R[i][j]) + 1e-6f;
        }
    }
    for (int i = 0; i < 3; i++) {
        float ra = _halfSize[i];
        float rb = other._halfSize[0] * AbsR[i][0] + other._halfSize[1] * AbsR[i][1] + other._halfSize[2] * AbsR[i][2];
        float distance = glm::abs(glm::dot(centerDiff, axesA[i]));
        if (distance > ra + rb) return result;
        float overlap = ra + rb - distance;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minOverlapAxis = axesA[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        float ra = _halfSize[0] * AbsR[0][i] + _halfSize[1] * AbsR[1][i] + _halfSize[2] * AbsR[2][i];
        float rb = other._halfSize[i];
        float distance = glm::abs(glm::dot(centerDiff, axesB[i]));
        if (distance > ra + rb) return result;
        float overlap = ra + rb - distance;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minOverlapAxis = axesB[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 crossAxis = glm::cross(axesA[i], axesB[j]);
            float length = glm::length(crossAxis);
            if (length < 1e-6f) continue;
            crossAxis /= length;
            float ra = _halfSize[0] * glm::abs(glm::dot(crossAxis, axesA[0])) +
                      _halfSize[1] * glm::abs(glm::dot(crossAxis, axesA[1])) +
                      _halfSize[2] * glm::abs(glm::dot(crossAxis, axesA[2]));
            float rb = other._halfSize[0] * glm::abs(glm::dot(crossAxis, axesB[0])) +
                      other._halfSize[1] * glm::abs(glm::dot(crossAxis, axesB[1])) +
                      other._halfSize[2] * glm::abs(glm::dot(crossAxis, axesB[2]));
            float distance = glm::abs(glm::dot(centerDiff, crossAxis));
            if (distance > ra + rb) return result;
            float overlap = ra + rb - distance;
            if (overlap < minOverlap) {
                minOverlap = overlap;
                minOverlapAxis = crossAxis;
            }
        }
    }
    result.intersected = true;
    if (glm::dot(minOverlapAxis, centerDiff) < 0) minOverlapAxis = -minOverlapAxis;
    result.normal = glm::normalize(minOverlapAxis);
    result.point = _center + centerDiff * 0.5f + result.normal * (minOverlap * 0.5f);
    return result;
}

IntersectionResult OBB::intersect(const Triangle &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 v0 = other.getV0(), v1 = other.getV1(), v2 = other.getV2();
    v0 -= _center; v1 -= _center; v2 -= _center;
    glm::vec3 axes[3] = {_rotationMatrix[0], _rotationMatrix[1], _rotationMatrix[2]};
    glm::vec3 e0 = v1 - v0, e1 = v2 - v1, e2 = v0 - v2;
    glm::vec3 edges[3] = {e0, e1, e2};
    glm::vec3 triangleNormal = glm::normalize(glm::cross(e0, -e2));
    float minOverlap = FLT_MAX;
    glm::vec3 minOverlapAxis(0.0f); 
    for (int i = 0; i < 3; i++) {
        float minT = glm::dot(axes[i], v0);
        float maxT = minT;
        float temp = glm::dot(axes[i], v1);
        minT = glm::min(minT, temp);
        maxT = glm::max(maxT, temp);
        temp = glm::dot(axes[i], v2);
        minT = glm::min(minT, temp);
        maxT = glm::max(maxT, temp);
        float extent = _halfSize[i];
        if (maxT < -extent || minT > extent) return result;
        float overlap = glm::min(maxT + extent, extent) - glm::max(minT - extent, -extent);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minOverlapAxis = axes[i];
        }
    }
    float minT = glm::dot(triangleNormal, v0);
    float maxT = minT;
    float temp = glm::dot(triangleNormal, v1);
    minT = glm::min(minT, temp);
    maxT = glm::max(maxT, temp);
    temp = glm::dot(triangleNormal, v2);
    minT = glm::min(minT, temp);
    maxT = glm::max(maxT, temp);
    float radius = _halfSize[0] * glm::abs(glm::dot(triangleNormal, axes[0])) +
                   _halfSize[1] * glm::abs(glm::dot(triangleNormal, axes[1])) +
                   _halfSize[2] * glm::abs(glm::dot(triangleNormal, axes[2]));
    if (maxT < -radius || minT > radius) return result;
    float overlap = glm::min(maxT + radius, radius) - glm::max(minT - radius, -radius);
    if (overlap < minOverlap) {
        minOverlap = overlap;
        minOverlapAxis = triangleNormal;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 axis = glm::cross(edges[i], axes[j]);
            float length = glm::length(axis);
            if (length < 1e-6f) continue;
            axis /= length;
            minT = glm::dot(axis, v0);
            maxT = minT;
            temp = glm::dot(axis, v1);
            minT = glm::min(minT, temp);
            maxT = glm::max(maxT, temp);
            temp = glm::dot(axis, v2);
            minT = glm::min(minT, temp);
            maxT = glm::max(maxT, temp);
            float radiusOBB = _halfSize[0] * glm::abs(glm::dot(axis, axes[0])) +
                             _halfSize[1] * glm::abs(glm::dot(axis, axes[1])) +
                             _halfSize[2] * glm::abs(glm::dot(axis, axes[2]));
            if (maxT < -radiusOBB || minT > radiusOBB) return result;
            overlap = glm::min(maxT + radiusOBB, radiusOBB) - glm::max(minT - radiusOBB, -radiusOBB);
            if (overlap < minOverlap) {
                minOverlap = overlap;
                minOverlapAxis = axis;
            }
        }
    }
    result.intersected = true;
    glm::vec3 worldV0 = other.getV0(), worldV1 = other.getV1(), worldV2 = other.getV2();
    glm::vec3 closestPoint = Triangle::closestPointOnTriangle(_center, worldV0, worldV1, worldV2);
    result.point = closestPoint;
    glm::vec3 direction = closestPoint - _center;
    float dist = glm::length(direction);
    if (dist > 1e-6f) result.normal = direction / dist;
    else result.normal = minOverlapAxis;
    return result;
}

IntersectionResult OBB::intersect(const MeshCollider &other) const {
    return Collider::inverse(other.intersect(*this));
}