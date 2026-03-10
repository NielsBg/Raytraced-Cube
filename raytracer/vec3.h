#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {} // Default constructor

    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {} // Constructor

    Vec3 operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
    } // Addition

    Vec3 operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
    } // Subtraction

    Vec3 operator*(float s) const {
    return Vec3(x * s, y * s, z * s);
    }// Scalar multiplication

    float dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
    }// Dot product

    Vec3 cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
    }// Cross product

    float length() const {
    return std::sqrt(x*x + y*y + z*z);
    }// Length

    Vec3 normalized() const {
    float len = length();
    return Vec3(x / len, y / len, z / len);
    }// Normalized vector
};