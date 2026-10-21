#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "matrix.h"
#include "object3d.h"

class Transform : public Object3D
{
public:
    Transform(Matrix& m, Object3D* obj);
    virtual bool intersect(const Ray& ray, Hit& hit, float tmin);

private:
    Matrix inverse_transform;
    Matrix normal_transform;
    Object3D* object;
};

#endif
