#ifndef COLLIDER_HPP_
    #define COLLIDER_HPP_

     #include <glm/glm.hpp>

typedef struct IntersectionResult {
    bool intersected;
    glm::vec3 point;
    glm::vec3 normal;
};

class AABB;
class Sphere;
class OBB;
class Triangle;
class MeshCollider;

class Collider {
    public:
        Collider() = default;
        virtual ~Collider() = 0;
        IntersectionResult isTouchedBy(const Collider &other) const;
        virtual IntersectionResult intersect(const AABB &other) const = 0;
        virtual IntersectionResult intersect(const Sphere &other) const = 0;
        virtual IntersectionResult intersect(const OBB &other) const = 0;
        virtual IntersectionResult intersect(const Triangle &other) const = 0;
        virtual IntersectionResult intersect(const MeshCollider &other) const = 0;
        static IntersectionResult inverse(const IntersectionResult &result);
    protected:
    private:
};

#endif
