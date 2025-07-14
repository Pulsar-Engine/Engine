#ifndef COLLISION_UTILS_HPP_
#define COLLISION_UTILS_HPP_

#include "../ECS/Core/Coordinator.hpp"
#include "../ECS/Components/ColliderComponent.hpp"
#include "../ECS/Components/TransformComponent.hpp"
#include <vector>

/**
 * Utilitaires pour faciliter la gestion des collisions
 */
class CollisionUtils {
public:
    /**
     * Trouve toutes les entités en collision avec une entité donnée
     */
    static std::vector<Entity> FindCollisionsForEntity(Entity entity, Coordinator& coordinator) {
        std::vector<Entity> collisions;
        
        if (!coordinator.HasComponent<ColliderComponent>(entity)) {
            return collisions;
        }
        
        auto& entityCollider = coordinator.GetComponent<ColliderComponent>(entity);
        
        // Parcourir toutes les autres entités avec des colliders
        for (Entity other = 0; other < MAX_ENTITIES; ++other) {
            if (other == entity || !coordinator.HasComponent<ColliderComponent>(other)) {
                continue;
            }
            
            auto& otherCollider = coordinator.GetComponent<ColliderComponent>(other);
            
            if (entityCollider.CheckIntersection(otherCollider).intersected) {
                collisions.push_back(other);
            }
        }
        
        return collisions;
    }
    
    /**
     * Vérifie si une entité est dans une zone trigger spécifique
     */
    static bool IsInTrigger(Entity entity, Entity trigger, Coordinator& coordinator) {
        if (!coordinator.HasComponent<ColliderComponent>(entity) || 
            !coordinator.HasComponent<ColliderComponent>(trigger)) {
            return false;
        }
        
        auto& entityCollider = coordinator.GetComponent<ColliderComponent>(entity);
        auto& triggerCollider = coordinator.GetComponent<ColliderComponent>(trigger);
        
        if (!triggerCollider.isTrigger) {
            return false;
        }
        
        return entityCollider.CheckIntersection(triggerCollider).intersected;
    }
    
    /**
     * Crée un collider AABB centré sur une position avec une taille donnée
     */
    static ColliderComponent CreateCenteredAABB(const glm::vec3& center, const glm::vec3& size, bool isTrigger = false) {
        glm::vec3 halfSize = size * 0.5f;
        return ColliderComponent::CreateAABB(center - halfSize, center + halfSize, isTrigger);
    }
    
    /**
     * Met à jour la position d'un collider selon son TransformComponent
     */
    static void UpdateColliderFromTransform(Entity entity, Coordinator& coordinator) {
        if (!coordinator.HasComponent<TransformComponent>(entity) || 
            !coordinator.HasComponent<ColliderComponent>(entity)) {
            return;
        }
        
        auto& transform = coordinator.GetComponent<TransformComponent>(entity);
        auto& colliderComp = coordinator.GetComponent<ColliderComponent>(entity);
        
        if (!colliderComp.HasCollider()) {
            return;
        }
        
        Collider* collider = colliderComp.collider.get();
        
        if (auto* aabb = dynamic_cast<AABB*>(collider)) {
            glm::vec3 size = aabb->max - aabb->min;
            aabb->min = transform.position - size * 0.5f;
            aabb->max = transform.position + size * 0.5f;
        }
        else if (auto* sphere = dynamic_cast<Sphere*>(collider)) {
            sphere->center = transform.position;
        }
        else if (auto* obb = dynamic_cast<OBB*>(collider)) {
            obb->center = transform.position;
            // Note: Pour une vraie implémentation, il faudrait aussi mettre à jour l'orientation
        }
    }
    
    /**
     * Obtient la distance entre deux entités avec colliders
     */
    static float GetDistanceBetween(Entity entityA, Entity entityB, Coordinator& coordinator) {
        if (!coordinator.HasComponent<TransformComponent>(entityA) || 
            !coordinator.HasComponent<TransformComponent>(entityB)) {
            return -1.0f; // Erreur
        }
        
        auto& transformA = coordinator.GetComponent<TransformComponent>(entityA);
        auto& transformB = coordinator.GetComponent<TransformComponent>(entityB);
        
        return glm::distance(transformA.position, transformB.position);
    }
    
    /**
     * Vérifie si une entité est dans un rayon donné d'une autre
     */
    static bool IsWithinRadius(Entity center, Entity target, float radius, Coordinator& coordinator) {
        float distance = GetDistanceBetween(center, target, coordinator);
        return distance >= 0.0f && distance <= radius;
    }
    
    /**
     * Trouve toutes les entités dans un rayon donné autour d'une position
     */
    static std::vector<Entity> FindEntitiesInRadius(const glm::vec3& center, float radius, Coordinator& coordinator) {
        std::vector<Entity> entities;
        
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            if (!coordinator.HasComponent<TransformComponent>(entity)) {
                continue;
            }
            
            auto& transform = coordinator.GetComponent<TransformComponent>(entity);
            float distance = glm::distance(center, transform.position);
            
            if (distance <= radius) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }
};

#endif /* !COLLISION_UTILS_HPP_ */
