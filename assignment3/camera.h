#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"

class Ray;

class Camera 
{
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;
};

class OrthographicCamera: public Camera 
{
public:
    OrthographicCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float size);

    Ray generateRay(Vec2f point);
    float getTMin() const;
    void glInit(int w, int h);
    void glPlaceCamera(void);
    void dollyCamera(float dist);
    void truckCamera(float dx, float dy);
    void rotateCamera(float rx, float ry);

private:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    float size;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float angle);

    Ray generateRay(Vec2f point);
    float getTMin() const;
    void glInit(int w, int h);
    void glPlaceCamera(void);
    void dollyCamera(float dist);
    void truckCamera(float dx, float dy);
    void rotateCamera(float rx, float ry);

private:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    float angle;
};

#endif
