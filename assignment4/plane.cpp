#include "plane.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include <math.h>
#include <GL/glut.h>

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

void Plane::paint()
{
    material->glSetMaterial();

    // 你的平面方程是 normal · P = offset。
    // normal 已在构造函数中归一化。
    Vec3f origin = normal * offset;

    Vec3f v = (std::fabs(normal.x()) < 0.9f)
        ? Vec3f(1, 0, 0) : Vec3f(0, 1, 0);

    Vec3f b1, b2;
    Vec3f::Cross3(b1, v, normal);
    b1.Normalize();
    Vec3f::Cross3(b2, normal, b1);
    b2.Normalize();

    const float big = 1000.0f;
    Vec3f p0 = origin - b1 * big - b2 * big;
    Vec3f p1 = origin + b1 * big - b2 * big;
    Vec3f p2 = origin + b1 * big + b2 * big;
    Vec3f p3 = origin - b1 * big + b2 * big;

    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(p0.x(), p0.y(), p0.z());
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glVertex3f(p3.x(), p3.y(), p3.z());
    glEnd();
}