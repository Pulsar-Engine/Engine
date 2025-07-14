#include "Sphere.hpp"
#include "AABB.hpp"
#include "OBB.hpp"
#include "Triangle.hpp"

Sphere::Sphere(const glm::vec3 &center, float radius) : _center(center), _radius(radius) {}

IntersectionResult Sphere::intersect(const Sphere &other) const {
    IntersectionResult result = {0};
    result.intersected = true;
    glm::vec3 direction = other.getCenter() - _center;
    float distanceSquared = glm::dot(direction, direction);
    float radiusSum = _radius + other.getRadius();

    if (distanceSquared > radiusSum * radiusSum) {
        result.intersected = false;
        return result;
    }
    result.point = _center + direction * (_radius / glm::length(direction));
    result.normal = glm::normalize(direction);
    return result;
}

IntersectionResult Sphere::intersect(const AABB &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult Sphere::intersect(const OBB &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 axes[3] = {other.getRotationMatrix()[0], other.getRotationMatrix()[1], other.getRotationMatrix()[2]};
    glm::vec3 localSphereCenter = _center - other.getCenter();
    glm::vec3 closestPoint = other.getCenter();
    for (int i = 0; i < 3; i++) {
        float projection = glm::dot(localSphereCenter, axes[i]);
        float clampedProjection = glm::clamp(projection, -other.getHalfSize()[i], other.getHalfSize()[i]);
        closestPoint += clampedProjection * axes[i];
    }
    glm::vec3 direction = closestPoint - _center;
    float distanceSquared = glm::dot(direction, direction);
    if (distanceSquared > _radius * _radius) return result;
    result.intersected = true;
    result.point = closestPoint;
    float distance = glm::sqrt(distanceSquared);
    if (distance > 1e-6f) {
        result.normal = direction / distance;
    } else {
        float minPenetration = FLT_MAX;
        int minAxis = 0;

        for (int i = 0; i < 3; i++) {
            float projection = glm::abs(glm::dot(localSphereCenter, axes[i]));
            float penetration = other.getHalfSize()[i] - projection;

            if (penetration < minPenetration) {
                minPenetration = penetration;
                minAxis = i;
            }
        }
        result.normal = axes[minAxis];
        if (glm::dot(localSphereCenter, axes[minAxis]) < 0) result.normal = -result.normal;
    }
    return result;
}

IntersectionResult Sphere::intersect(const Triangle &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 closestPoint = Triangle::closestPointOnTriangle(_center, other.getV0(), other.getV1(), other.getV2());
    glm::vec3 direction = closestPoint - _center;
    float distanceSquared = glm::dot(direction, direction);
    if (distanceSquared > _radius * _radius) return result;
    result.intersected = true;
    result.point = closestPoint;
    float distance = glm::sqrt(distanceSquared);
    if (distance > 1e-6f) result.normal = direction / distance;
    else {
        glm::vec3 e1 = other.getV1() - other.getV0();
        glm::vec3 e2 = other.getV2() - other.getV0();
        result.normal = glm::normalize(glm::cross(e1, e2));
    }
    return result;
}

IntersectionResult Sphere::intersect(const MeshCollider &other) const {
    return Collider::inverse(other.intersect(*this));
}