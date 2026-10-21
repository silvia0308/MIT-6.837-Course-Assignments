#include "ray.h"
#include "camera.h"
#include <math.h>
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

PerspectiveCamera::PerspectiveCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float angle) : center(center), direction(direction), up(up), angle(angle)
{
    this->direction.Normalize();

    Vec3f::Cross3(horizontal, this->direction, this->up);
    horizontal.Normalize();

    Vec3f::Cross3(this->up, horizontal, this->direction);
    this->up.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    float screen_scale = std::tan(angle * 0.5f);
    Vec3f ray_direction = direction
        + (2.0f * point.x() - 1.0f) * screen_scale * horizontal
        + (2.0f * point.y() - 1.0f) * screen_scale * up;
    ray_direction.Normalize();
    return Ray(center, ray_direction);
}

float PerspectiveCamera::getTMin() const
{
    return 0.0f;
}
