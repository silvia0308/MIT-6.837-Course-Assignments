#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object3d.h"
#include "vectors.h"

class Sphere: public Object3D 
{
public:
	Sphere(const Vec3f& c, float r, Material* m);
	virtual bool intersect(const Ray& r, Hit& h, float tmin);

private:
	Vec3f center;
	float radius;
};

#endif
