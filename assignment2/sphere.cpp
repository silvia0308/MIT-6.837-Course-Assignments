#include "sphere.h"
#include "material.h"
#include "ray.h"
#include "hit.h"

Sphere::Sphere(const Vec3f& c, float r, Material* m): center(c), radius(r) {
    material = m;
}

bool Sphere::intersect(const Ray& r, Hit& h, float tmin) {
    Vec3f oc = r.getOrigin() - center;
    float a = r.getDirection().Dot3(r.getDirection());
    float b = 2.0f * oc.Dot3(r.getDirection());
    float c = oc.Dot3(oc) - radius * radius;
    float disc = b * b - 4.0f * a * c;
    if (disc < 0.0f) return false;
    float sq = sqrt(disc);
    float t = (-b - sq) / (2.0f * a);
    if (t < tmin) {
        t = (-b + sq) / (2.0f * a);
        if (t < tmin) return false;
    }
    if (t < h.getT()) {
        h.set(t, material, r);
        return true;
    }
    return false;
}
