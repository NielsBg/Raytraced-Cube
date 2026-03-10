#pragma once
#include "vec3.h"
#include "ray.h"
#include <limits>

struct Triangle {
    Vec3 v0, v1, v2;

    Triangle(const Vec3& a, const Vec3& b, const Vec3& c)
        : v0(a), v1(b), v2(c) {}

    float intersect(const Ray& ray) const {
        const float EPS = 1e-6f;

        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;

        Vec3 h = ray.direction.cross(edge2);
        float a = edge1.dot(h);

        if (a > -EPS && a < EPS)
            return -1.0f; // Ray is parallel

        float f = 1.0f / a;
        Vec3 s = ray.origin - v0;
        float u = f * s.dot(h);

        if (u < 0.0f || u > 1.0f)
            return -1.0f;

        Vec3 q = s.cross(edge1);
        float v = f * ray.direction.dot(q);

        if (v < 0.0f || u + v > 1.0f)
            return -1.0f;

        float t = f * edge2.dot(q);

        return (t > EPS) ? t : -1.0f;
    }
};