#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"
#include "ray.h"

class Camera 
{
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
};

class OrthographicCamera: public Camera 
{
public:
    OrthographicCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float size);

    Ray generateRay(Vec2f point);
    float getTMin() const;

private:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    float size;
};

#endif
