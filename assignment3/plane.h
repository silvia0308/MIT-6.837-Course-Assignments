#ifndef _PLANE_H_
#define _PLANE_H_

#include "object3d.h"
#include "vectors.h"

class Material;

class Plane : public Object3D
{
public:
    Plane(const Vec3f& normal, float d, Material* m);
    bool intersect(const Ray& ray, Hit& hit, float tmin);
    void paint(void);

private:
    Vec3f normal;
    float offset;
};

#endif
