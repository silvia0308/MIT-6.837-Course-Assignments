#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "object3d.h"
#include "matrix.h"

class Transform : public Object3D
{
public:
    Transform(Matrix& m, Object3D* obj);
    bool intersect(const Ray& ray, Hit& hit, float tmin);
    void paint(void);

private:
    Matrix transform;
    Matrix inverse_transform;
    Matrix normal_transform;
    Object3D* object;
};

#endif
