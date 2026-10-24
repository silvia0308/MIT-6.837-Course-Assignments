#include "material.h"
#include "glCanvas.h"
#include "ray.h"
#include "hit.h"
#include <math.h>
#include <windows.h>
#include <GL/gl.h>

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

PhongMaterial::PhongMaterial(const Vec3f& diffuse, const Vec3f& specular, float exp): Material(diffuse), specularColor(specular), exponent(exp) {}

void PhongMaterial::glSetMaterial(void) const {

    GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat specular[4] = {
      getSpecularColor().r(),
      getSpecularColor().g(),
      getSpecularColor().b(),
      1.0 };
    GLfloat diffuse[4] = {
      getDiffuseColor().r(),
      getDiffuseColor().g(),
      getDiffuseColor().b(),
      1.0 };

    // NOTE: GL uses the Blinn Torrance version of Phong...      
    float glexponent = exponent;
    if (glexponent < 0) glexponent = 0;
    if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

    // OPTIONAL: 3 pass rendering to fix the specular highlight 
    // artifact for small specular exponents (wide specular lobe)

    if (SPECULAR_FIX_WHICH_PASS == 0) {
        // First pass, draw only the specular highlights
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

    }
    else if (SPECULAR_FIX_WHICH_PASS == 1) {
        // Second pass, compute normal dot light 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }
    else {
        // Third pass, add ambient & diffuse terms
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }

#endif
}

Vec3f PhongMaterial::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
{
    Vec3f n = hit.getNormal();
    n.Normalize();

    Vec3f l = dirToLight;
    l.Normalize();

    float ndotl = n.Dot3(l);
    if (ndotl <= 0.0f)
        return Vec3f(0.0f, 0.0f, 0.0f);

    Vec3f v = ray.getDirection() * -1.0f; // 从交点朝向观察者
    v.Normalize();

    Vec3f h = l + v;                      // Blinn 半程向量
    float specular = 0.0f;
    if (h.Length() > 0.0f) {
        h.Normalize();
        float ndoth = n.Dot3(h);
        if (ndoth > 0.0f)
            specular = std::pow(ndoth, exponent);
    }

    #if SPECULAR_FIX
        specular *= ndotl;
    #endif
    return lightColor *
        (diffuseColor * ndotl + specularColor * specular);
}

