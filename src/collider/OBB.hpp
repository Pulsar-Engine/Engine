#ifndef OBB_HPP_
    #define OBB_HPP_

    #include "Collider.hpp"

class OBB : public Collider {
    public:
        OBB(const glm::vec3 &center, const glm::vec3 &halfSize, const glm::mat3 &rotationMatrix);
        IntersectionResult intersect(const AABB &other) const override;
        IntersectionResult intersect(const Sphere &other) const override;
        IntersectionResult intersect(const OBB &other) const override;
        IntersectionResult intersect(const Triangle &other) const override;
        IntersectionResult intersect(const MeshCollider &other) const override;
        glm::vec3 getCenter() const { return _center; }
        glm::vec3 getHalfSize() const { return _halfSize; }
        glm::mat3 getRotationMatrix() const { return _rotationMatrix; }
    protected:
        glm::vec3 _center;
        glm::vec3 _halfSize;
        glm::mat3 _rotationMatrix;
    private:
};

#endif /* !OBB_HPP_ */
