#include "MeshCollider.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "OBB.hpp"
#include "Triangle.hpp"

MeshCollider::MeshCollider(const Mesh &mesh) : _mesh(mesh) {}

IntersectionResult MeshCollider::intersect(const AABB &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    const std::vector<Vertex>& vertices = _mesh.getVertices();
    const std::vector<uint32_t>& indices = _mesh.getIndices();
    glm::mat4 modelMatrix = _mesh.getModelMatrix();
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i]].pos, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 1]].pos, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 2]].pos, 1.0f));
        Triangle triangle(v0, v1, v2);
        IntersectionResult triangleResult = other.intersect(triangle);
        if (triangleResult.intersected) {
            result = triangleResult;
            result.normal = -result.normal;
            break;
        }
    }
    return result;
}

IntersectionResult MeshCollider::intersect(const Sphere &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    const std::vector<Vertex>& vertices = _mesh.getVertices();
    const std::vector<uint32_t>& indices = _mesh.getIndices();
    glm::mat4 modelMatrix = _mesh.getModelMatrix();
    float minDistance = FLT_MAX;
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i]].pos, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 1]].pos, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 2]].pos, 1.0f));
        Triangle triangle(v0, v1, v2);
        IntersectionResult triangleResult = other.intersect(triangle);
        if (triangleResult.intersected) {
            float distance = glm::length(triangleResult.point - other.getCenter());
            if (distance < minDistance) {
                minDistance = distance;
                result = triangleResult;
                result.normal = -result.normal;
            }
        }
    }
    return result;
}

IntersectionResult MeshCollider::intersect(const OBB &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    const std::vector<Vertex>& vertices = _mesh.getVertices();
    const std::vector<uint32_t>& indices = _mesh.getIndices();
    glm::mat4 modelMatrix = _mesh.getModelMatrix();
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i]].pos, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 1]].pos, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 2]].pos, 1.0f));
        Triangle triangle(v0, v1, v2);
        IntersectionResult triangleResult = other.intersect(triangle);
        if (triangleResult.intersected) {
            result = triangleResult;
            result.normal = -result.normal;
            break;
        }
    }
    return result;
}

IntersectionResult MeshCollider::intersect(const Triangle &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    const std::vector<Vertex>& vertices = _mesh.getVertices();
    const std::vector<uint32_t>& indices = _mesh.getIndices();
    glm::mat4 modelMatrix = _mesh.getModelMatrix();
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i]].pos, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 1]].pos, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i + 2]].pos, 1.0f));
        Triangle triangle(v0, v1, v2);
        IntersectionResult triangleResult = triangle.intersect(other);
        if (triangleResult.intersected) {
            result = triangleResult;
            break;
        }
    }
    return result;
}

IntersectionResult MeshCollider::intersect(const MeshCollider &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    const std::vector<Vertex>& vertices1 = _mesh.getVertices();
    const std::vector<uint32_t>& indices1 = _mesh.getIndices();
    glm::mat4 modelMatrix1 = _mesh.getModelMatrix();
    const std::vector<Vertex>& vertices2 = other._mesh.getVertices();
    const std::vector<uint32_t>& indices2 = other._mesh.getIndices();
    glm::mat4 modelMatrix2 = other._mesh.getModelMatrix();
    for (size_t i = 0; i < indices1.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(modelMatrix1 * glm::vec4(vertices1[indices1[i]].pos, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix1 * glm::vec4(vertices1[indices1[i + 1]].pos, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix1 * glm::vec4(vertices1[indices1[i + 2]].pos, 1.0f));
        Triangle triangle1(v0, v1, v2);
        for (size_t j = 0; j < indices2.size(); j += 3) {
            glm::vec3 u0 = glm::vec3(modelMatrix2 * glm::vec4(vertices2[indices2[j]].pos, 1.0f));
            glm::vec3 u1 = glm::vec3(modelMatrix2 * glm::vec4(vertices2[indices2[j + 1]].pos, 1.0f));
            glm::vec3 u2 = glm::vec3(modelMatrix2 * glm::vec4(vertices2[indices2[j + 2]].pos, 1.0f));
            Triangle triangle2(u0, u1, u2);
            IntersectionResult triangleResult = triangle1.intersect(triangle2);
            if (triangleResult.intersected) {
                result = triangleResult;
                return result;
            }
        }
    }
    return result;
}