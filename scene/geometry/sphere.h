#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <scene/geometry/geometry.h>
#include <glm/vec3.hpp>
#include <QColor>

class Sphere : public Geometry {
public:
    Sphere(QString name, Transform transform, Material *material);
    ~Sphere();
    float getT(float a, float b, float c) const;
    bool intersect(const Ray &ray, Intersection *intersection) const;
    void mapNormal(Intersection &isect) const;
    glm::vec3 getColor(Intersection &isect) const;
    float getAlpha(float y, float py, float qy) const;
    QRgb slerp(float alpha, QRgb az, QRgb bz) const;
    glm::vec3 sphereInterpolation(Intersection &isect, QImage *attrib) const;
};

#endif // __SPHERE_H__
