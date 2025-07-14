#ifndef SPHERE_HPP_
    #define SPHERE_HPP_

    #include "Collider.hpp"

class Sphere : public Collider {
    public:
        Sphere(const glm::vec3 &center, float radius);
        IntersectionResult intersect(const Sphere &other) const override;
        IntersectionResult intersect(const AABB &other) const override;
        IntersectionResult intersect(const OBB &other) const override;
        IntersectionResult intersect(const Triangle &other) const override;
        IntersectionResult intersect(const MeshCollider &other) const override;
        glm::vec3 getCenter() const { return _center; }
        float getRadius() const { return _radius; }
        void setCenter(const glm::vec3& center) { _center = center; }
        void setRadius(float radius) { _radius = radius; }
    protected:
        glm::vec3 _center;
        float _radius;
    private:
};

#endif /* !SPHERE_HPP_ */
