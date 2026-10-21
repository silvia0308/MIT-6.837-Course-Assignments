#include <math.h>

#include "plane.h"

namespace {
const float kIntersectionEpsilon = 1e-6f;
}

Plane::Plane(const Vec3f& n, float d, Material* m): normal(n), offset(d)
{
    material = m;

    float normal_length = normal.Length();
    if (normal_length > kIntersectionEpsilon) {
        normal /= normal_length;
        offset /= normal_length;
    }
}

bool Plane::intersect(const Ray& ray, Hit& hit, float tmin)
{
    float denominator = normal.Dot3(ray.getDirection());
    if (std::fabs(denominator) <= kIntersectionEpsilon) {
        return false;
    }

    float t = (offset - normal.Dot3(ray.getOrigin())) / denominator;
    if (t < tmin || t >= hit.getT()) {
        return false;
    }

    hit.set(t, material, normal, ray);
    return true;
}
