#ifndef TRIANGLE_HPP_
    #define TRIANGLE_HPP_

    #include "Collider.hpp"

class Triangle : public Collider {
    public:
        Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);
        IntersectionResult intersect(const AABB &other) const override;
        IntersectionResult intersect(const Sphere &other) const override;
        IntersectionResult intersect(const OBB &other) const override;
        IntersectionResult intersect(const Triangle &other) const override;
        IntersectionResult intersect(const MeshCollider &other) const override;
        glm::vec3 getV0() const { return _v0; }
        glm::vec3 getV1() const { return _v1; }
        glm::vec3 getV2() const { return _v2; }
        
        // Méthode statique utilitaire pour calculer le point le plus proche sur un triangle
        static glm::vec3 closestPointOnTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        
    protected:
        glm::vec3 _v0;
        glm::vec3 _v1;
        glm::vec3 _v2;
    private:
        // Méthodes auxiliaires pour l'intersection triangle-triangle
        bool testCoplanarTriangles(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                 const glm::vec3& u0, const glm::vec3& u1, const glm::vec3& u2) const;
        bool pointInTriangle2D(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) const;
        bool computeInterval(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                           const glm::vec3& direction, float d0, float d1, float d2,
                           float& t_min, float& t_max) const;
        glm::vec3 findPointOnLine(const glm::vec3& direction, const glm::vec3& normal, float d) const;
};
#endif /* !TRIANGLE_HPP_ */
