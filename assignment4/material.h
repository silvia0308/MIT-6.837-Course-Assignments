#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"

class Ray;
class Hit;

class Material 
{
public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual Vec3f getReflectiveColor() const = 0;
  virtual Vec3f getTransparentColor() const = 0;
  virtual float getIndexOfRefraction() const = 0;
  virtual void glSetMaterial(void) const = 0;
  virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const = 0;

protected:

  // REPRESENTATION
  Vec3f diffuseColor;
  
};

class PhongMaterial: public Material 
{
public:
	PhongMaterial(const Vec3f& diffuseColor, const Vec3f& specularColor, float exponent, const Vec3f& reflectiveColor, const Vec3f& transparentColor, float indexOfRefraction);

	Vec3f getSpecularColor() const { return specularColor; }
	Vec3f getReflectiveColor() const { return reflectiveColor; }
	Vec3f getTransparentColor() const { return transparentColor; }
	float getIndexOfRefraction() const { return indexOfRefraction; }
	void glSetMaterial(void) const;
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const;

private:
    Vec3f specularColor;
	Vec3f reflectiveColor;
	Vec3f transparentColor;
	float exponent;
	float indexOfRefraction;
};

#endif
