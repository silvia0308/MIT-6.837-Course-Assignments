#include "group.h"
#include "ray.h"
#include "hit.h"

Group::Group(int num_objects): objects(new Object3D* [num_objects]), num_objects(num_objects) {}

Group::~Group() {
    // objects are owned elsewhere (the scene parser / main),
    // so here we only free the array itself.
    delete[] objects;
}

void Group::addObject(int index, Object3D* obj) {
    objects[index] = obj;
}

bool Group::intersect(const Ray& r, Hit& h, float tmin) {
    bool intersected = false;
    for (int i = 0; i < num_objects; i++) {
        intersected |= objects[i]->intersect(r, h, tmin);
    }
    return intersected;
}
