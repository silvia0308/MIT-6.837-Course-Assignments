#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object3d.h"
#include "vectors.h"

class Triangle : public Object3D
{
public:
    Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, Material* m);
    virtual bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
    Vec3f vertex0;
    Vec3f edge1;
    Vec3f edge2;
    Vec3f normal;
};

#endif
