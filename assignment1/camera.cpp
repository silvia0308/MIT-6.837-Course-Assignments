#include "ray.h"
#include "camera.h"
#include <float.h>

OrthographicCamera::OrthographicCamera(const Vec3f &center, const Vec3f &direction, const Vec3f &up, float size): center(center), direction(direction), up(up), size(size)
{
    this->direction.Normalize();

    Vec3f::Cross3(horizontal, this->direction, this->up);
    horizontal.Normalize();

    Vec3f::Cross3(this->up, horizontal, this->direction);
    this->up.Normalize();
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    Vec3f origin = center + (point.x() - 0.5f) * size * horizontal + (point.y() - 0.5f) * size * up;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
    return -FLT_MAX;
}
