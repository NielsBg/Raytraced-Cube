#pragma once
#include "vec3.h"
#include "ray.h"
#include <cmath>

struct Camera {
    Vec3 origin;
    Vec3 forward;
    Vec3 right;
    Vec3 up;

    float viewportHeight;
    float viewportWidth;

    Camera(float fovDegrees, float aspectRatio) {
        origin = Vec3(2, 1, -5);

        Vec3 lookAt(0, 0, 0);
        forward = (lookAt - origin).normalized();

        Vec3 worldUp(0, 1, 0);
        right = forward.cross(worldUp).normalized();
        up = right.cross(forward).normalized();

        float theta = fovDegrees * 3.14159265f / 180.0f;
        float h = std::tan(theta / 2.0f);

        viewportHeight = 2.0f * h;
        viewportWidth = aspectRatio * viewportHeight;
    }

    Ray getRay(float u, float v) const {
        Vec3 direction =
            forward +
            right * (u * viewportWidth) +
            up * (v * viewportHeight);

        return Ray(origin, direction);
    }
};