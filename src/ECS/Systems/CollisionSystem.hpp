#ifndef COLLISION_SYSTEM_HPP_
#define COLLISION_SYSTEM_HPP_

#include "System.hpp"
#include "../Components/ColliderComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/VelocityComponent.hpp"
#include "../Core/Coordinator.hpp"
#include <vector>
#include <functional>
#include <iostream>

// Structure pour stocker les informations d'une collision
struct CollisionInfo {
    Entity entityA;
    Entity entityB;
    IntersectionResult intersection;
    bool isTrigger;  // True si au moins un des colliders est un trigger
};

// Callback pour les événements de collision
using CollisionCallback = std::function<void(const CollisionInfo&)>;
using TriggerCallback = std::function<void(Entity trigger, Entity other)>;

class CollisionSystem : public System {
public:
    void Update(float dt, Coordinator& coordinator) override {
        (void)dt; // Unused parameter
        
        // Vider la liste des collisions précédentes
        _currentCollisions.clear();
        
        // Convertir le set en vector pour les indices
        std::vector<Entity> entityList(entities.begin(), entities.end());
        
        // Tester toutes les paires d'entités
        for (size_t i = 0; i < entityList.size(); ++i) {
            for (size_t j = i + 1; j < entityList.size(); ++j) {
                Entity entityA = entityList[i];
                Entity entityB = entityList[j];
                
                // Vérifier la collision entre ces deux entités
                CheckCollisionBetween(entityA, entityB, coordinator);
            }
        }
        
        // Traiter toutes les collisions détectées
        ProcessCollisions(coordinator);
    }
    
    // Enregistrer des callbacks pour les collisions
    void RegisterCollisionCallback(CollisionCallback callback) {
        _collisionCallbacks.push_back(callback);
    }
    
    void RegisterTriggerCallback(TriggerCallback callback) {
        _triggerCallbacks.push_back(callback);
    }
    
    // Obtenir les collisions de cette frame
    const std::vector<CollisionInfo>& GetCurrentCollisions() const {
        return _currentCollisions;
    }
    
    // Vérifier si une entité spécifique a des collisions
    std::vector<CollisionInfo> GetCollisionsForEntity(Entity entity) const {
        std::vector<CollisionInfo> result;
        for (const auto& collision : _currentCollisions) {
            if (collision.entityA == entity || collision.entityB == entity) {
                result.push_back(collision);
            }
        }
        return result;
    }

private:
    std::vector<CollisionInfo> _currentCollisions;
    std::vector<CollisionCallback> _collisionCallbacks;
    std::vector<TriggerCallback> _triggerCallbacks;
    
    void CheckCollisionBetween(Entity entityA, Entity entityB, Coordinator& coordinator) {
        std::cout << "CheckCollisionBetween(" << entityA << ", " << entityB << ")" << std::endl;
        
        // Vérifier que les deux entités ont des colliders
        if (!coordinator.HasComponent<ColliderComponent>(entityA) || 
            !coordinator.HasComponent<ColliderComponent>(entityB)) {
            std::cout << "Une des entités n'a pas de ColliderComponent" << std::endl;
            return;
        }
        
        std::cout << "Les deux entités ont des ColliderComponent" << std::endl;
        
        auto& colliderA = coordinator.GetComponent<ColliderComponent>(entityA);
        auto& colliderB = coordinator.GetComponent<ColliderComponent>(entityB);
        
        std::cout << "ColliderA type: " << colliderA.GetTypeName() << std::endl;
        std::cout << "ColliderB type: " << colliderB.GetTypeName() << std::endl;
        
        // Vérifier que les colliders sont valides
        if (!colliderA.HasCollider() || !colliderB.HasCollider()) {
            std::cout << "Un des colliders n'est pas valide" << std::endl;
            return;
        }
        
        std::cout << "Les deux colliders sont valides" << std::endl;
        
        // TEMPORAIRE: Désactiver UpdateColliderPositions pour isoler le bug
        // UpdateColliderPositions(entityA, entityB, coordinator);
        
        // Tester l'intersection avec vérifications supplémentaires
        IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
        
        std::cout << "Avant CheckIntersection..." << std::endl;
        
        try {
            if (colliderA.collider && colliderB.collider) {
                // Test sécurisé d'intersection
                result = colliderA.CheckIntersection(colliderB);
                std::cout << "CheckIntersection terminé, résultat: " << (result.intersected ? "collision" : "pas de collision") << std::endl;
            }
        } catch (const std::exception& e) {
            // En cas d'erreur, on assume pas de collision
            std::cerr << "Erreur dans CheckIntersection: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Erreur inconnue dans CheckIntersection" << std::endl;
        }
        
        if (result.intersected) {
            std::cout << "Collision détectée, ajout à la liste" << std::endl;
            // Créer l'info de collision
            CollisionInfo collision;
            collision.entityA = entityA;
            collision.entityB = entityB;
            collision.intersection = result;
            collision.isTrigger = colliderA.isTrigger || colliderB.isTrigger;
            
            _currentCollisions.push_back(collision);
        }
        
        std::cout << "CheckCollisionBetween terminé" << std::endl;
    }
    
    void ProcessCollisions(Coordinator& coordinator) {
        for (const auto& collision : _currentCollisions) {
            if (collision.isTrigger) {
                // Traitement des triggers
                ProcessTriggerCollision(collision, coordinator);
            } else {
                // Traitement des collisions physiques
                ProcessPhysicalCollision(collision, coordinator);
            }
            
            // Appeler les callbacks
            for (auto& callback : _collisionCallbacks) {
                callback(collision);
            }
        }
    }
    
    void ProcessTriggerCollision(const CollisionInfo& collision, Coordinator& coordinator) {
        (void)coordinator; // Unused parameter
        
        // Les triggers ne causent pas de réponse physique
        // Juste appeler les callbacks de trigger
        for (auto& callback : _triggerCallbacks) {
            auto& colliderA = coordinator.GetComponent<ColliderComponent>(collision.entityA);
            auto& colliderB = coordinator.GetComponent<ColliderComponent>(collision.entityB);
            
            if (colliderA.isTrigger) {
                callback(collision.entityA, collision.entityB);
            }
            if (colliderB.isTrigger) {
                callback(collision.entityB, collision.entityA);
            }
        }
    }
    
    void ProcessPhysicalCollision(const CollisionInfo& collision, Coordinator& coordinator) {
        auto& colliderA = coordinator.GetComponent<ColliderComponent>(collision.entityA);
        auto& colliderB = coordinator.GetComponent<ColliderComponent>(collision.entityB);
        
        // Si l'un des objets est statique, seul l'autre bouge
        bool aIsStatic = colliderA.isStatic;
        bool bIsStatic = colliderB.isStatic;
        
        if (aIsStatic && bIsStatic) {
            return; // Deux objets statiques ne se repoussent pas
        }
        
        // Séparer les objets
        if (coordinator.HasComponent<TransformComponent>(collision.entityA) && 
            coordinator.HasComponent<TransformComponent>(collision.entityB)) {
            
            SeparateEntities(collision, coordinator, aIsStatic, bIsStatic);
        }
        
        // Appliquer la réponse de vélocité si les entités ont des composants de vélocité
        if (coordinator.HasComponent<Velocity>(collision.entityA) && 
            coordinator.HasComponent<Velocity>(collision.entityB)) {
            
            ApplyCollisionResponse(collision, coordinator, aIsStatic, bIsStatic);
        }
    }
    
    void SeparateEntities(const CollisionInfo& collision, Coordinator& coordinator, bool aIsStatic, bool bIsStatic) {
        auto& transformA = coordinator.GetComponent<TransformComponent>(collision.entityA);
        auto& transformB = coordinator.GetComponent<TransformComponent>(collision.entityB);
        
        glm::vec3 separation = collision.intersection.normal * 0.01f; // Petite séparation
        
        if (!aIsStatic && !bIsStatic) {
            // Les deux objets bougent
            transformA.position += separation * 0.5f;
            transformB.position -= separation * 0.5f;
        } else if (!aIsStatic) {
            // Seul A bouge
            transformA.position += separation;
        } else if (!bIsStatic) {
            // Seul B bouge
            transformB.position -= separation;
        }
    }
    
    void ApplyCollisionResponse(const CollisionInfo& collision, Coordinator& coordinator, bool aIsStatic, bool bIsStatic) {
        auto& colliderA = coordinator.GetComponent<ColliderComponent>(collision.entityA);
        auto& colliderB = coordinator.GetComponent<ColliderComponent>(collision.entityB);
        auto& velocityA = coordinator.GetComponent<Velocity>(collision.entityA);
        auto& velocityB = coordinator.GetComponent<Velocity>(collision.entityB);
        
        // Calcul de la réponse de collision basée sur la restitution
        float restitution = std::min(colliderA.restitution, colliderB.restitution);
        glm::vec3 normal = collision.intersection.normal;
        
        // Vitesses relatives
        glm::vec3 relativeVelocity = velocityA.linearVelocity - velocityB.linearVelocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, normal);
        
        // Ne pas résoudre si les objets se séparent déjà
        if (velocityAlongNormal > 0) {
            return;
        }
        
        // Calcul de l'impulsion
        float impulse = -(1 + restitution) * velocityAlongNormal;
        
        if (!aIsStatic && !bIsStatic) {
            impulse /= (1.0f / colliderA.mass + 1.0f / colliderB.mass);
            glm::vec3 impulseVector = impulse * normal;
            
            velocityA.linearVelocity += impulseVector / colliderA.mass;
            velocityB.linearVelocity -= impulseVector / colliderB.mass;
        } else if (!aIsStatic) {
            velocityA.linearVelocity += impulse * normal;
        } else if (!bIsStatic) {
            velocityB.linearVelocity -= impulse * normal;
        }
    }
    
    void UpdateColliderPositions(Entity entityA, Entity entityB, Coordinator& coordinator) {
        UpdateSingleColliderPosition(entityA, coordinator);
        UpdateSingleColliderPosition(entityB, coordinator);
    }
    
    void UpdateSingleColliderPosition(Entity entity, Coordinator& coordinator) {
        // VERSION AVEC PROTECTIONS RENFORCÉES
        try {
            if (!coordinator.HasComponent<TransformComponent>(entity) || 
                !coordinator.HasComponent<ColliderComponent>(entity)) {
                return;
            }
            
            auto& transform = coordinator.GetComponent<TransformComponent>(entity);
            auto& colliderComp = coordinator.GetComponent<ColliderComponent>(entity);
            
            if (!colliderComp.HasCollider() || !colliderComp.collider) {
                return;
            }
            
            // Mettre à jour la position du collider selon le transform
            Collider* collider = colliderComp.collider.get();
            
            if (auto* aabb = dynamic_cast<AABB*>(collider)) {
                // Pour AABB, on utilise les méthodes setter appropriées
                glm::vec3 size = aabb->getMax() - aabb->getMin();
                glm::vec3 center = transform.position;
                
                // Vérifications de sécurité
                if (glm::length(size) < 1e-6f) {
                    // Taille trop petite, utiliser une taille par défaut
                    size = glm::vec3(0.1f);
                }
                
                aabb->setMin(center - size * 0.5f);
                aabb->setMax(center + size * 0.5f);
            }
            else if (auto* sphere = dynamic_cast<Sphere*>(collider)) {
                // Pour Sphere, on utilise le setter approprié
                sphere->setCenter(transform.position);
            }
            else if (auto* obb = dynamic_cast<OBB*>(collider)) {
                // Pour OBB, on utilise le setter approprié
                obb->setCenter(transform.position);
                // TODO: Mettre à jour l'orientation selon transform.rotation
            }
            // Triangle et MeshCollider sont plus complexes, on les skip pour l'instant
        } catch (const std::exception& e) {
            std::cerr << "Erreur dans UpdateSingleColliderPosition: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Erreur inconnue dans UpdateSingleColliderPosition" << std::endl;
        }
    }
};

#endif /* !COLLISION_SYSTEM_HPP_ */
