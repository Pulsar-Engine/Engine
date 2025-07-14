#ifndef AABB_HPP_
    #define AABB_HPP_

    #include "Collider.hpp"

class AABB : public Collider {
    public:
        AABB(const glm::vec3 &min, const glm::vec3 &max);
        IntersectionResult intersect(const AABB &other) const override;
        IntersectionResult intersect(const Sphere &other) const override;
        IntersectionResult intersect(const OBB &other) const override;
        IntersectionResult intersect(const Triangle &other) const override; 
        IntersectionResult intersect(const MeshCollider &other) const override;
        glm::vec3 getMin() const { return _min; }
        glm::vec3 getMax() const { return _max; }
        void setMin(const glm::vec3& min) { _min = min; }
        void setMax(const glm::vec3& max) { _max = max; }
    protected:
        glm::vec3 _min;
        glm::vec3 _max;
    private:
};

#endif /* !AABB_HPP_ */
