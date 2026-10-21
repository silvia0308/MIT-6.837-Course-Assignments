#include <assert.h>

#include "transform.h"

Transform::Transform(Matrix& m, Object3D* obj): object(obj)
{
    assert(object != NULL);

    int invertible = m.Inverse(inverse_transform);
    assert(invertible);
    inverse_transform.Transpose(normal_transform);
}

bool Transform::intersect(const Ray& ray, Hit& hit, float tmin)
{
    Vec3f object_origin = ray.getOrigin();
    Vec3f object_direction = ray.getDirection();
    inverse_transform.Transform(object_origin);
    inverse_transform.TransformDirection(object_direction);
    Ray object_ray(object_origin, object_direction);

    Hit object_hit(hit);
    if (!object->intersect(object_ray, object_hit, tmin)) {
        return false;
    }

    Vec3f world_normal = object_hit.getNormal();
    normal_transform.TransformDirection(world_normal);
    world_normal.Normalize();

    hit.set(object_hit.getT(), object_hit.getMaterial(), world_normal, ray);
    return true;
}
