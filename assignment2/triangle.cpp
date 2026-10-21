#include <math.h>

#include "triangle.h"

namespace {
const float kIntersectionEpsilon = 1e-6f;
}

Triangle::Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, Material* m) : vertex0(a), edge1(b - a), edge2(c - a)
{
    material = m;

    Vec3f::Cross3(normal, edge1, edge2);
    normal.Normalize();
}

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin)
{
    Vec3f pvec;
    Vec3f::Cross3(pvec, ray.getDirection(), edge2);
    float determinant = edge1.Dot3(pvec);
    if (std::fabs(determinant) <= kIntersectionEpsilon) {
        return false;
    }

    float inverse_determinant = 1.0f / determinant;
    Vec3f tvec = ray.getOrigin() - vertex0;
    float u = tvec.Dot3(pvec) * inverse_determinant;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Vec3f qvec;
    Vec3f::Cross3(qvec, tvec, edge1);
    float v = ray.getDirection().Dot3(qvec) * inverse_determinant;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float t = edge2.Dot3(qvec) * inverse_determinant;
    if (t < tmin || t >= hit.getT()) {
        return false;
    }

    hit.set(t, material, normal, ray);
    return true;
}
