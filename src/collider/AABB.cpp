#include "AABB.hpp"
#include "Sphere.hpp"
#include "OBB.hpp"
#include "Triangle.hpp"

AABB::AABB(const glm::vec3 &min, const glm::vec3 &max) : _min(min), _max(max) {}

IntersectionResult AABB::intersect(const Sphere &other) const {
    IntersectionResult result = {0};
    result.intersected = true;
    glm::vec3 closestPoint = glm::clamp(other.getCenter(), _min, _max);
    glm::vec3 direction = closestPoint - other.getCenter();
    float distanceSquared = glm::dot(direction, direction);
    if (distanceSquared > other.getRadius() * other.getRadius()) {
        result.intersected = false;
        return result;
    }
    result.point = closestPoint;
    result.normal = glm::normalize(direction);
    return result;
}

IntersectionResult AABB::intersect(const AABB &other) const {
    IntersectionResult result = {0};
    result.intersected = true;
    if (_min.x > other._max.x || _max.x < other._min.x ||
        _min.y > other._max.y || _max.y < other._min.y ||
        _min.z > other._max.z || _max.z < other._min.z) {
        result.intersected = false;
        return result;
    }
    result.point = glm::vec3(std::max(_min.x, other._min.x), std::max(_min.y, other._min.y), std::max(_min.z, other._min.z));
    result.normal = glm::normalize(result.point - _min);
    return result;
}

IntersectionResult AABB::intersect(const OBB &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 obbAxes[3] = {other.getRotationMatrix()[0], other.getRotationMatrix()[1], other.getRotationMatrix()[2]};
    glm::vec3 aabbAxes[3] = {glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1)};
    glm::vec3 aabbCenter = (_min + _max) * 0.5f;
    glm::vec3 aabbExtents = (_max - _min) * 0.5f;
    glm::vec3 centerDiff = other.getCenter() - aabbCenter;
    float minOverlap = FLT_MAX;
    glm::vec3 minOverlapAxis(0.0f);
    for (int i = 0; i < 3; i++) {
        float obbProjection = other.getHalfSize()[0] * glm::abs(glm::dot(aabbAxes[i], obbAxes[0])) +
                             other.getHalfSize()[1] * glm::abs(glm::dot(aabbAxes[i], obbAxes[1])) +
                             other.getHalfSize()[2] * glm::abs(glm::dot(aabbAxes[i], obbAxes[2]));
        float aabbProjection = aabbExtents[i];
        float distance = glm::abs(glm::dot(centerDiff, aabbAxes[i]));
        if (distance > aabbProjection + obbProjection) return result;
        float overlap = aabbProjection + obbProjection - distance;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minOverlapAxis = aabbAxes[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        float aabbProjection = aabbExtents[0] * glm::abs(glm::dot(obbAxes[i], aabbAxes[0])) +
                              aabbExtents[1] * glm::abs(glm::dot(obbAxes[i], aabbAxes[1])) +
                              aabbExtents[2] * glm::abs(glm::dot(obbAxes[i], aabbAxes[2]));
        float obbProjection = other.getHalfSize()[i];
        float distance = glm::abs(glm::dot(centerDiff, obbAxes[i]));
        if (distance > aabbProjection + obbProjection) return result;
        float overlap = aabbProjection + obbProjection - distance;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minOverlapAxis = obbAxes[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 crossAxis = glm::cross(aabbAxes[i], obbAxes[j]);
            float length = glm::length(crossAxis);
            if (length < 1e-6f) continue;
            crossAxis /= length;
            float aabbProjection = aabbExtents[0] * glm::abs(glm::dot(crossAxis, aabbAxes[0])) +
                                  aabbExtents[1] * glm::abs(glm::dot(crossAxis, aabbAxes[1])) +
                                  aabbExtents[2] * glm::abs(glm::dot(crossAxis, aabbAxes[2]));
            float obbProjection = other.getHalfSize()[0] * glm::abs(glm::dot(crossAxis, obbAxes[0])) +
                                 other.getHalfSize()[1] * glm::abs(glm::dot(crossAxis, obbAxes[1])) +
                                 other.getHalfSize()[2] * glm::abs(glm::dot(crossAxis, obbAxes[2]));
            float distance = glm::abs(glm::dot(centerDiff, crossAxis));
            if (distance > aabbProjection + obbProjection) return result;
            float overlap = aabbProjection + obbProjection - distance;
            if (overlap < minOverlap) {
                minOverlap = overlap;
                minOverlapAxis = crossAxis;
            }
        }
    }
    result.intersected = true;
    if (glm::dot(minOverlapAxis, centerDiff) < 0) minOverlapAxis = -minOverlapAxis;
    result.normal = glm::normalize(minOverlapAxis);
    result.point = aabbCenter + centerDiff * 0.5f + result.normal * (minOverlap * 0.5f);
    return result;
}

IntersectionResult AABB::intersect(const Triangle &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 v0 = other.getV0(), v1 = other.getV1(), v2 = other.getV2();
    glm::vec3 e0 = v1 - v0, e1 = v2 - v1, e2 = v0 - v2;
    glm::vec3 triangleNormal = glm::normalize(glm::cross(e0, -e2));
    float minProj = glm::dot(triangleNormal, v0);
    float maxProj = minProj;
    float temp = glm::dot(triangleNormal, v1);
    minProj = glm::min(minProj, temp);
    maxProj = glm::max(maxProj, temp);
    temp = glm::dot(triangleNormal, v2);
    minProj = glm::min(minProj, temp);
    maxProj = glm::max(maxProj, temp);
    glm::vec3 center = (_min + _max) * 0.5f;
    glm::vec3 extents = (_max - _min) * 0.5f;
    float radius = extents.x * glm::abs(triangleNormal.x) + extents.y * glm::abs(triangleNormal.y) + extents.z * glm::abs(triangleNormal.z);
    float centerProj = glm::dot(triangleNormal, center);
    if (centerProj - radius > maxProj || centerProj + radius < minProj) return result;
    glm::vec3 axes[3] = {glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1)};
    for (int i = 0; i < 3; i++) {
        float minT = glm::dot(axes[i], v0);
        float maxT = minT;
        temp = glm::dot(axes[i], v1);
        minT = glm::min(minT, temp);
        maxT = glm::max(maxT, temp);
        temp = glm::dot(axes[i], v2);
        minT = glm::min(minT, temp);
        maxT = glm::max(maxT, temp);
        float minB = _min[i];
        float maxB = _max[i];
        if (maxB < minT || minB > maxT) return result;
    }
    glm::vec3 edges[3] = {e0, e1, e2};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 axis = glm::cross(edges[i], axes[j]);
            float axisLength = glm::length(axis);
            if (axisLength < 1e-6f) continue;
            axis /= axisLength;
            float minT = glm::dot(axis, v0);
            float maxT = minT;
            temp = glm::dot(axis, v1);
            minT = glm::min(minT, temp);
            maxT = glm::max(maxT, temp);
            temp = glm::dot(axis, v2);
            minT = glm::min(minT, temp);
            maxT = glm::max(maxT, temp);
            float radiusB = extents.x * glm::abs(axis.x) + extents.y * glm::abs(axis.y) + extents.z * glm::abs(axis.z);
            float centerProjB = glm::dot(axis, center);
            if (centerProjB - radiusB > maxT || centerProjB + radiusB < minT) return result;
        }
    }
    result.intersected = true;
    glm::vec3 closestPoint = Triangle::closestPointOnTriangle(center, v0, v1, v2);
    result.point = closestPoint;
    glm::vec3 direction = closestPoint - center;
    float length = glm::length(direction);
    if (length > 1e-6f) result.normal = direction / length;
    else result.normal = triangleNormal;
    return result;
}

IntersectionResult AABB::intersect(const MeshCollider &other) const {
    return Collider::inverse(other.intersect(*this));
}