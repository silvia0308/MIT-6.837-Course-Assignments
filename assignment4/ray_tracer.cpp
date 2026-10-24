#include "ray_tracer.h"
#include "group.h"
#include "material.h"
#include "light.h"
#include "ray.h"
#include "hit.h"
#include "rayTree.h"
#include <float.h>
#include <math.h>
#include <vector>

#if TRANSPARENT_SHADOWS
namespace {
// Treat transparentColor as transmission through one world-space unit.
// The equivalent Beer-law absorption coefficient is -log(transmission).
float transmissionOverDistance(float unitTransmission, float distance) {
    if (distance <= 0.0f) return 1.0f;
    if (unitTransmission <= 0.0f) return 0.0f;
    if (unitTransmission >= 1.0f) return 1.0f;
    return std::pow(unitTransmission, distance);
}

Vec3f transmissionOverDistance(const Vec3f& unitTransmission, float distance) {
    return Vec3f(
        transmissionOverDistance(unitTransmission.r(), distance),
        transmissionOverDistance(unitTransmission.g(), distance),
        transmissionOverDistance(unitTransmission.b(), distance));
}
}
#endif

RayTracer::RayTracer(SceneParser* s, int max_bounces, float cutoff_weight, bool shadows, bool shade_back): sceneParser(s), maxBounces(max_bounces), cutoffWeight(cutoff_weight), shadows(shadows), shadeBack(shade_back){}

Vec3f RayTracer::mirrorDirection(const Vec3f& normal, const Vec3f& incoming)
{
    Vec3f n = normal;
    Vec3f d = incoming;
    n.Normalize();
    d.Normalize();

    Vec3f reflected = d - n * (2.0f * d.Dot3(n));
    reflected.Normalize();
    return reflected;
}

bool RayTracer::transmittedDirection(const Vec3f& normal, const Vec3f& incoming, float index_i, float index_t, Vec3f& transmitted)
{
    if (index_i <= 0.0f || index_t <= 0.0f) {
        return false;
    }

    Vec3f n = normal;
    Vec3f d = incoming;
    n.Normalize();
    d.Normalize();

    // 让法线始终指向入射射线所在的一侧。
    if (d.Dot3(n) > 0.0f) {
        n.Negate();
    }

    const float cos_i = -d.Dot3(n);
    const float eta = index_i / index_t;
    const float k = 1.0f -
        eta * eta * (1.0f - cos_i * cos_i);

    if (k < 0.0f) {
        return false; // 全内反射
    }

    transmitted =
        d * eta +
        n * (eta * cos_i -
            static_cast<float>(std::sqrt(k)));
    transmitted.Normalize();
    return true;
}

Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces,
    float weight, float indexOfRefraction,
    Hit& hit) const {
    const float epsilon = 1e-4f;
    const Vec3f black(0.0f, 0.0f, 0.0f);

    // 每次调用都初始化自己的交点。递归调用会传入另一个 Hit。
    hit = Hit(FLT_MAX, nullptr, black);

    if (bounces > maxBounces || weight < cutoffWeight) {
        return black;
    }

    bool intersected =
        sceneParser->getGroup()->intersect(ray, hit, tmin);

    if (bounces == 0) {
        RayTree::SetMainSegment(
            ray, 0.0f, intersected ? hit.getT() : 100.0f);
    }

    if (!intersected) {
        return sceneParser->getBackgroundColor();
    }

    Material* material = hit.getMaterial();
    Vec3f point = hit.getIntersectionPoint();

    // 保留原始 hit 的法线，必要时只翻转用于局部着色的法线。
    Hit shadingHit = hit;
    Vec3f shadingNormal = hit.getNormal();
    if (shadeBack &&
        ray.getDirection().Dot3(shadingNormal) > 0.0f) {
        shadingNormal.Negate();
        shadingHit.set(hit.getT(), material, shadingNormal, ray);
    }

    Vec3f color =
        sceneParser->getAmbientLight() * material->getDiffuseColor();

    for (int i = 0; i < sceneParser->getNumLights(); ++i) {
        Vec3f lightDirection;
        Vec3f lightColor;
        float distanceToLight;

        sceneParser->getLight(i)->getIllumination(
            point, lightDirection, lightColor, distanceToLight);

        if (shadows) {
            Vec3f shadowDirection = lightDirection;
            shadowDirection.Normalize();

            Ray shadowRay(
                point + shadowDirection * epsilon,
                shadowDirection);
#if TRANSPARENT_SHADOWS
            Vec3f visibility(1.0f, 1.0f, 1.0f);
            float distanceTraveled = epsilon;
            std::vector<Material*> activeMedia;

            // The ray starts just inside the surface if it points inward.
            if (material->getTransparentColor().Length() > 0.0f &&
                shadowDirection.Dot3(hit.getNormal()) < 0.0f) {
                activeMedia.push_back(material);
            }

            // Integrate absorption over each segment inside transparent media.
            while (distanceTraveled < distanceToLight) {
                const float remaining = distanceToLight - distanceTraveled;
                Hit shadowHit(remaining, nullptr, black);
                const bool intersected = sceneParser->getGroup()->intersect(
                    shadowRay, shadowHit, epsilon);
                const float segmentLength = intersected
                    ? shadowHit.getT() : remaining;

                RayTree::AddShadowSegment(
                    shadowRay, 0.0f, segmentLength);

                for (Material* medium : activeMedia) {
                    visibility = visibility * transmissionOverDistance(
                        medium->getTransparentColor(), segmentLength);
                }
                if (visibility.Length() <= 1e-6f) {
                    break;
                }

                if (!intersected) {
                    break;
                }

                Material* boundaryMaterial = shadowHit.getMaterial();
                if (boundaryMaterial->getTransparentColor().Length() <= 0.0f) {
                    visibility = black;
                    break;
                }

                const float crossing =
                    shadowDirection.Dot3(shadowHit.getNormal());
                if (crossing < 0.0f) {
                    activeMedia.push_back(boundaryMaterial);
                }
                else if (crossing > 0.0f) {
                    for (auto it = activeMedia.begin();
                         it != activeMedia.end(); ++it) {
                        if (*it == boundaryMaterial) {
                            activeMedia.erase(it);
                            break;
                        }
                    }
                }

                const float step = shadowHit.getT() + epsilon;
                distanceTraveled += step;
                shadowRay = Ray(
                    shadowRay.pointAtParameter(step),
                    shadowDirection);
            }

            if (visibility.Length() <= 1e-6f) {
                continue;
            }
            lightColor = lightColor * visibility;
#else
            Hit shadowHit(distanceToLight, nullptr, black);

            bool blocked = sceneParser->getGroup()->intersect(
                shadowRay, shadowHit, epsilon);

            RayTree::AddShadowSegment(
                shadowRay, 0.0f,
                blocked ? shadowHit.getT() : distanceToLight);

            if (blocked) {
                continue;
            }
#endif
        }

        color += material->Shade(
            ray, shadingHit, lightDirection, lightColor);
    }

    // 最深层仍计算上面的直接光照，但不再生成子射线。
    if (bounces >= maxBounces) {
        return color;
    }

    Vec3f direction = ray.getDirection();
    direction.Normalize();

    Vec3f normal = hit.getNormal();
    normal.Normalize();

    const bool entering = direction.Dot3(normal) < 0.0f;
    if (!entering) {
        normal.Negate();
    }

    Vec3f reflectedDirection =
        direction - normal * (2.0f * direction.Dot3(normal));
    reflectedDirection.Normalize();

    Vec3f reflectiveColor = material->getReflectiveColor();
    Vec3f transparentColor = material->getTransparentColor();

    if (transparentColor.Length() > 0.0f) {
        const float nextIOR = entering
            ? material->getIndexOfRefraction()
            : 1.0f;

        const float eta = indexOfRefraction / nextIOR;
        const float cosI = -direction.Dot3(normal);
        const float k =
            1.0f - eta * eta * (1.0f - cosI * cosI);

        if (k < 0.0f) {
            // 全内反射：透明部分也沿反射方向传播。
            reflectiveColor += transparentColor;
        }
        else {
            Vec3f refractedDirection =
                direction * eta +
                normal * (eta * cosI -
                    static_cast<float>(std::sqrt(k)));
            refractedDirection.Normalize();

            Ray refractedRay(
                point + refractedDirection * epsilon,
                refractedDirection);
            Hit refractedHit(FLT_MAX, nullptr, black);

            Vec3f transmittedColor = traceRay(
                refractedRay,
                epsilon,
                bounces + 1,
                weight * transparentColor.Length(),
                nextIOR,
                refractedHit);

            RayTree::AddTransmittedSegment(
                refractedRay,
                0.0f,
                refractedHit.getMaterial() != nullptr
                ? refractedHit.getT()
                : 100.0f);

            color += transparentColor * transmittedColor;
        }
    }

    if (reflectiveColor.Length() > 0.0f) {
        Ray reflectedRay(
            point + reflectedDirection * epsilon,
            reflectedDirection);
        Hit reflectedHit(FLT_MAX, nullptr, black);

        Vec3f reflectedColor = traceRay(
            reflectedRay,
            epsilon,
            bounces + 1,
            weight * reflectiveColor.Length(),
            indexOfRefraction,
            reflectedHit);

        RayTree::AddReflectedSegment(
            reflectedRay,
            0.0f,
            reflectedHit.getMaterial() != nullptr
            ? reflectedHit.getT()
            : 100.0f);

        color += reflectiveColor * reflectedColor;
    }

    return color;
}

