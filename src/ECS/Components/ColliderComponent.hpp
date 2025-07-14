#ifndef COLLIDER_COMPONENT_HPP_
#define COLLIDER_COMPONENT_HPP_

#include "../../collider/Collider.hpp"
#include "../../collider/AABB.hpp"
#include "../../collider/Sphere.hpp"
#include "../../collider/OBB.hpp"
#include "../../collider/Triangle.hpp"
#include "../../collider/MeshCollider.hpp"
#include <memory>
#include <string>

struct ColliderComponent {
    std::unique_ptr<Collider> collider;
    bool isTrigger = false;
    bool isStatic = false;
    float mass = 1.0f;
    float restitution = 0.5f;
    float friction = 0.3f;
    
    // Constructeurs par défaut
    ColliderComponent() = default;
    
    // Interdire la copie
    ColliderComponent(const ColliderComponent&) = delete;
    ColliderComponent& operator=(const ColliderComponent&) = delete;
    
    // Autoriser le move
    ColliderComponent(ColliderComponent&&) = default;
    ColliderComponent& operator=(ColliderComponent&&) = default;
    
    static ColliderComponent CreateAABB(const glm::vec3& min, const glm::vec3& max, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::make_unique<AABB>(min, max);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    static ColliderComponent CreateSphere(const glm::vec3& center, float radius, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::make_unique<Sphere>(center, radius);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    static ColliderComponent CreateOBB(const glm::vec3& center, const glm::vec3& size, const glm::mat3& orientation, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::make_unique<OBB>(center, size, orientation);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    static ColliderComponent CreateTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::make_unique<Triangle>(a, b, c);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    static ColliderComponent CreateMeshCollider(const Mesh& mesh, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::make_unique<MeshCollider>(mesh);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    static ColliderComponent CreateFromCollider(std::unique_ptr<Collider> collider, bool isTrigger = false) {
        ColliderComponent comp;
        comp.collider = std::move(collider);
        comp.isTrigger = isTrigger;
        return comp;
    }
    
    bool HasCollider() const {
        return collider != nullptr;
    }
    
    IntersectionResult CheckIntersection(const ColliderComponent& other) const {
        if (!HasCollider() || !other.HasCollider()) {
            return {false, glm::vec3(0.0f), glm::vec3(0.0f)};
        }
        return collider->isTouchedBy(*other.collider);
    }
    
    const char* GetTypeName() const {
        if (!collider) return "None";
        
        if (dynamic_cast<AABB*>(collider.get())) return "AABB";
        if (dynamic_cast<Sphere*>(collider.get())) return "Sphere";
        if (dynamic_cast<OBB*>(collider.get())) return "OBB";
        if (dynamic_cast<Triangle*>(collider.get())) return "Triangle";
        if (dynamic_cast<MeshCollider*>(collider.get())) return "MeshCollider";
        return "Unknown";
    }
};

#endif /* !COLLIDER_COMPONENT_HPP_ */
