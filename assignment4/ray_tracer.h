#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "scene_parser.h"

// Optional colored shadows: 0 = opaque shadows, 1 = light passes through
// transparent materials and is tinted by their transparentColor.
#ifndef TRANSPARENT_SHADOWS
#define TRANSPARENT_SHADOWS 0
#endif

class Ray;
class Hit;

class RayTracer
{
public:
	RayTracer(SceneParser* s, int max_bounces, float cutoff_weight, bool shadows, bool shade_back);

	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit) const;
	Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming);
	bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming, float index_i, float index_t, Vec3f& transmitted);

private:
	SceneParser* sceneParser;
	int maxBounces;
	float cutoffWeight;
	bool shadows;
	bool shadeBack;
};

#endif
