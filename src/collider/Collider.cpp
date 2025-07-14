#include "Collider.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "OBB.hpp"
#include "Triangle.hpp"
#include "MeshCollider.hpp"

IntersectionResult Collider::isTouchedBy(const Collider &other) const {
    if (dynamic_cast<const AABB*>(&other))
        return intersect(static_cast<const AABB&>(other));
    if (dynamic_cast<const Sphere*>(&other))
        return intersect(static_cast<const Sphere&>(other));
    if (dynamic_cast<const OBB*>(&other))
        return intersect(static_cast<const OBB&>(other));
    if (dynamic_cast<const Triangle*>(&other))
        return intersect(static_cast<const Triangle&>(other));
    if (dynamic_cast<const MeshCollider*>(&other))
        return intersect(static_cast<const MeshCollider&>(other));
    return {false, glm::vec3(0.0f), glm::vec3(0.0f)};
}

IntersectionResult Collider::inverse(const IntersectionResult &result) {
    if (!result.intersected)
        return result;
    return {result.intersected, -result.point, -result.normal};
}
