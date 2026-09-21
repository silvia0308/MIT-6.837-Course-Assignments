#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"

class Group: public Object3D 
{
public:
	Group(int num_objects);
	virtual ~Group();

	void addObject(int index, Object3D* obj);
	virtual bool intersect(const Ray& r, Hit& h, float tmin);

private:
	int num_objects;
	Object3D **objects;   // array of Object3D* pointers
};

#endif
