#include "sphere.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include <math.h>
#include <GL/glut.h>

extern int theta_steps;
extern int phi_steps;
extern bool gouraud;

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
        Vec3f normal = r.pointAtParameter(t) - center;
        normal.Normalize();
        h.set(t, material, normal, r);
        return true;
    }
    return false;
}

void Sphere::paint()
{
    material->glSetMaterial();
    glShadeModel(gouraud ? GL_SMOOTH : GL_FLAT);

    const float pi = 3.14159265358979323846f;

    auto point = [&](int t, int p) {
        float theta = 2.0f * pi * t / theta_steps;
        float phi = -0.5f * pi + pi * p / phi_steps;
        float cp = std::cos(phi);

        return center + Vec3f(
            radius * cp * std::cos(theta),
            radius * std::sin(phi),
            radius * cp * std::sin(theta));
        };

    auto sendVertex = [&](const Vec3f& position, const Vec3f& faceNormal) {
        Vec3f n = faceNormal;
        if (gouraud) {
            n = position - center; // 球面上该顶点的真实法线
            n.Normalize();
        }
        glNormal3f(n.x(), n.y(), n.z());
        glVertex3f(position.x(), position.y(), position.z());
        };

    auto triangle = [&](const Vec3f& a, const Vec3f& b, const Vec3f& c) {
        Vec3f faceNormal;
        Vec3f::Cross3(faceNormal, b - a, c - a);
        faceNormal.Normalize();

        sendVertex(a, faceNormal);
        sendVertex(b, faceNormal);
        sendVertex(c, faceNormal);
        };

    glBegin(GL_TRIANGLES);
    for (int p = 0; p < phi_steps; ++p) {
        for (int t = 0; t < theta_steps; ++t) {
            Vec3f p00 = point(t, p);
            Vec3f p10 = point(t, p + 1);
            Vec3f p11 = point(t + 1, p + 1);
            Vec3f p01 = point(t + 1, p);

            // 两极只画一个三角形，避免退化三角形。
            if (p < phi_steps - 1)
                triangle(p00, p10, p11);
            if (p > 0)
                triangle(p00, p11, p01);
        }
    }
    glEnd();

    glShadeModel(GL_SMOOTH); // 恢复 GLCanvas 原来的设置
}