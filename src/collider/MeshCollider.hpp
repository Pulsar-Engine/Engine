#ifndef MESHCOLLIDER_HPP_
    #define MESHCOLLIDER_HPP_

    #include "Collider.hpp"
    #include "graphic/backend/vulkan/render/Mesh.hpp"

class MeshCollider : public Collider {
    public:
        MeshCollider(const Mesh &mesh);
        IntersectionResult intersect(const AABB &other) const override;
        IntersectionResult intersect(const Sphere &other) const override;
        IntersectionResult intersect(const OBB &other) const override;
        IntersectionResult intersect(const Triangle &other) const override;
        IntersectionResult intersect(const MeshCollider &other) const override;
        const Mesh& getMesh() const { return _mesh; }
    protected:
        const Mesh &_mesh;
    private:
    
};

#endif /* !MESHCOLLIDER_HPP_ */
