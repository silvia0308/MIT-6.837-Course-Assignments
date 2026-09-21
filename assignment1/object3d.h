#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "material.h"
#include "ray.h"
#include "hit.h"

class Object3D
{
public:
    Object3D() {}
    virtual ~Object3D() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

protected:
    Material* material;
};

#endif
