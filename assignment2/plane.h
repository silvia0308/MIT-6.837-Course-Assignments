#ifndef _PLANE_H_
#define _PLANE_H_

#include "object3d.h"
#include "vectors.h"

class Plane : public Object3D
{
public:
    Plane(const Vec3f& normal, float d, Material* m);
    virtual bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
    Vec3f normal;
    float offset;
};

#endif
