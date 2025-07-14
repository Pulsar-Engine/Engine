#include "Triangle.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "OBB.hpp"
#include "MeshCollider.hpp"

Triangle::Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
    : _v0(v0), _v1(v1), _v2(v2) {}

IntersectionResult Triangle::intersect(const AABB &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult Triangle::intersect(const Sphere &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult Triangle::intersect(const OBB &other) const {
    return Collider::inverse(other.intersect(*this));
}

IntersectionResult Triangle::intersect(const Triangle &other) const {
    IntersectionResult result = {false, glm::vec3(0.0f), glm::vec3(0.0f)};
    glm::vec3 v0 = _v0, v1 = _v1, v2 = _v2;
    glm::vec3 u0 = other.getV0(), u1 = other.getV1(), u2 = other.getV2();
    glm::vec3 n1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    glm::vec3 n2 = glm::normalize(glm::cross(u1 - u0, u2 - u0));
    float d1 = glm::dot(n1, v0);
    float d2 = glm::dot(n2, u0);
    float du0 = glm::dot(n1, u0) - d1;
    float du1 = glm::dot(n1, u1) - d1;
    float du2 = glm::dot(n1, u2) - d1;
    if ((du0 > 0 && du1 > 0 && du2 > 0) || (du0 < 0 && du1 < 0 && du2 < 0)) return result;
    float dv0 = glm::dot(n2, v0) - d2;
    float dv1 = glm::dot(n2, v1) - d2;
    float dv2 = glm::dot(n2, v2) - d2;
    if ((dv0 > 0 && dv1 > 0 && dv2 > 0) || (dv0 < 0 && dv1 < 0 && dv2 < 0)) return result;
    if (glm::abs(glm::dot(n1, n2)) > 0.999f) {
        result.intersected = testCoplanarTriangles(v0, v1, v2, u0, u1, u2);
        if (result.intersected) {
            result.point = (v0 + v1 + v2 + u0 + u1 + u2) / 6.0f;
            result.normal = n1;
        }
        return result;
    }
    glm::vec3 direction = glm::cross(n1, n2);
    float t1_min, t1_max, t2_min, t2_max;
    if (!computeInterval(v0, v1, v2, direction, dv0, dv1, dv2, t1_min, t1_max) ||
        !computeInterval(u0, u1, u2, direction, du0, du1, du2, t2_min, t2_max)) return result;
    if (t1_max < t2_min || t2_max < t1_min) return result;
    result.intersected = true;
    float t_intersect = (std::max(t1_min, t2_min) + std::min(t1_max, t2_max)) * 0.5f;
    glm::vec3 point_on_line = findPointOnLine(direction, n1, d1);
    result.point = point_on_line + t_intersect * glm::normalize(direction);
    result.normal = glm::normalize(n1 + n2);
    return result;
}

bool Triangle::testCoplanarTriangles(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                const glm::vec3& u0, const glm::vec3& u1, const glm::vec3& u2) const {
    return pointInTriangle2D(v0, u0, u1, u2) || pointInTriangle2D(v1, u0, u1, u2) || pointInTriangle2D(v2, u0, u1, u2) ||
           pointInTriangle2D(u0, v0, v1, v2) || pointInTriangle2D(u1, v0, v1, v2) || pointInTriangle2D(u2, v0, v1, v2);
}

bool Triangle::pointInTriangle2D(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) const {
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);
    float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

bool Triangle::computeInterval(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                        const glm::vec3& direction, float d0, float d1, float d2,
                        float& t_min, float& t_max) const {
    float p0 = glm::dot(v0, direction);
    float p1 = glm::dot(v1, direction);
    float p2 = glm::dot(v2, direction);
    if (d0 * d1 > 0) {
        t_min = p2;
        t_max = p2;
        if (d2 * d0 < 0) {
            float t = p0 + (p2 - p0) * d0 / (d0 - d2);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
            t = p1 + (p2 - p1) * d1 / (d1 - d2);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
        }
    } else if (d0 * d2 > 0) {
        t_min = p1;
        t_max = p1;
        if (d1 * d0 < 0) {
            float t = p0 + (p1 - p0) * d0 / (d0 - d1);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
            t = p2 + (p1 - p2) * d2 / (d2 - d1);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
        }
    } else {
        t_min = p0;
        t_max = p0;
        if (d0 * d1 < 0) {
            float t = p1 + (p0 - p1) * d1 / (d1 - d0);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
            t = p2 + (p0 - p2) * d2 / (d2 - d0);
            t_min = std::min(t_min, t);
            t_max = std::max(t_max, t);
        }
    }
    return t_max >= t_min;
}

glm::vec3 Triangle::findPointOnLine(const glm::vec3& direction, const glm::vec3& normal, float d) const {
    glm::vec3 abs_dir = glm::abs(direction);
    if (abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z) return glm::vec3(d / normal.x, 0.0f, 0.0f);
    else if (abs_dir.y >= abs_dir.z) return glm::vec3(0.0f, d / normal.y, 0.0f);
    else return glm::vec3(0.0f, 0.0f, d / normal.z);
}

glm::vec3 Triangle::closestPointOnTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = point - a;
    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;  
    glm::vec3 bp = point - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }
    glm::vec3 cp = point - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + w * ac;
    }
    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

IntersectionResult Triangle::intersect(const MeshCollider &other) const {
    return Collider::inverse(other.intersect(*this));
}
