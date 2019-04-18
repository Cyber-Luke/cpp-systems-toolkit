/**
 * @file Vector.hpp
 * @brief Comprehensive vector mathematics library for 2D and 3D operations
 * @author Lukas Ernst
 * 
 * High-performance vector mathematics library providing Vec2 and Vec3 classes with
 * full arithmetic support, geometric operations, interpolation, and graphics programming
 * utilities. Optimized for real-time applications and educational purposes.
 */

#define _USE_MATH_DEFINES
#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <array>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VectorMath {

/**
 * @brief 2D Vector class with comprehensive mathematical operations
 * 
 * Provides all basic vector operations including dot product, normalization,
 * length calculations, and operator overloads for intuitive mathematical syntax.
 */
struct Vec2 {
    float x, y;

    // Constructors
    Vec2() noexcept : x(0.0f), y(0.0f) {}
    Vec2(float x_val, float y_val) : x(x_val), y(y_val) {}
    Vec2(const Vec2& other) : x(other.x), y(other.y) {}

    // Utility methods
    inline bool IsZero() const noexcept { 
        return (std::abs(x) < 1e-6f && std::abs(y) < 1e-6f); 
    }

    inline float Length() const noexcept {
        return std::sqrt(x * x + y * y);
    }

    inline float LengthSquared() const noexcept {
        return x * x + y * y;
    }

    inline Vec2& Normalize() noexcept {
        const float length = Length();
        if (length > 1e-6f) {
            x /= length;
            y /= length;
        } else {
            x = y = 0.0f;
        }
        return *this;
    }

    inline float Dot(const Vec2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    // Operator overloads
    Vec2& operator*=(const Vec2& other) { x *= other.x; y *= other.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(const Vec2& other) { x /= other.x; y /= other.y; return *this; }
    Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }

    // Assignment operator
    Vec2& operator=(const Vec2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
};

// Vec2 binary operators
inline Vec2 operator*(Vec2 a, const Vec2& b) { return a *= b; }
inline Vec2 operator*(Vec2 a, float scalar) { return a *= scalar; }
inline Vec2 operator/(Vec2 a, const Vec2& b) { return a /= b; }
inline Vec2 operator/(Vec2 a, float scalar) { return a /= scalar; }
inline Vec2 operator+(Vec2 a, const Vec2& b) { return a += b; }
inline Vec2 operator-(Vec2 a, const Vec2& b) { return a -= b; }

/**
 * @brief 3D Vector class with comprehensive mathematical operations
 * 
 * Extends Vec2 functionality to three dimensions, including cross product,
 * distance calculations, and 3D-specific utility methods.
 */
struct Vec3 {
    float x, y, z;

    // Constructors
    Vec3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

    // Utility methods
    inline bool IsZero() const noexcept {
        return (std::abs(x) < 1e-6f && std::abs(y) < 1e-6f && std::abs(z) < 1e-6f);
    }

    inline float Length() const noexcept {
        return std::sqrt(x * x + y * y + z * z);
    }

    inline float Length2D() const noexcept {
        return std::sqrt(x * x + y * y);
    }

    inline float LengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }

    inline Vec3& Normalize() noexcept {
        const float length = Length();
        if (length > 1e-6f) {
            x /= length;
            y /= length;
            z /= length;
        } else {
            x = y = z = 0.0f;
        }
        return *this;
    }

    inline Vec3 Normalized() const noexcept {
        const float length = Length();
        if (length > 1e-6f) {
            return Vec3(x / length, y / length, z / length);
        }
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    inline float Dot(const Vec3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vec3 Cross(const Vec3& other) const noexcept {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Distance methods - implemented inline to avoid forward declaration issues
    inline float Distance(const Vec3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }

    inline float DistanceSquared(const Vec3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return dx*dx + dy*dy + dz*dz;
    }

    // Operator overloads
    Vec3& operator*=(const Vec3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/=(const Vec3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }
    Vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

    // Assignment operator
    Vec3& operator=(const Vec3& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
};

// Vec3 binary operators
inline Vec3 operator*(Vec3 a, const Vec3& b) { return a *= b; }
inline Vec3 operator*(Vec3 a, float scalar) { return a *= scalar; }
inline Vec3 operator/(Vec3 a, const Vec3& b) { return a /= b; }
inline Vec3 operator/(Vec3 a, float scalar) { return a /= scalar; }
inline Vec3 operator+(Vec3 a, const Vec3& b) { return a += b; }
inline Vec3 operator-(Vec3 a, const Vec3& b) { return a -= b; }

/**
 * @brief Utility functions for angle calculations (implemented in Vector.cpp)
 */
Vec2 CalculateAngle(const Vec3& from, const Vec3& to);
float CalculateFOV(const Vec2& current, const Vec2& target);
void ClampAngles(float angles[2]);

/**
 * @brief Interpolation functions (implemented in Vector.cpp)
 */
Vec3 Lerp(const Vec3& start, const Vec3& end, float t);
Vec2 SlerpAngles(const Vec2& start, const Vec2& end, float t);

/**
 * @brief High-performance template operations
 */
template<typename T>
inline void Add(const T& a, const T& b, T& result) {
    result = a + b;
}

template<typename T>
inline void Subtract(const T& a, const T& b, T& result) {
    result = a - b;
}

template<typename T>
inline void Scale(const T& vec, float scalar, T& result) {
    result = vec * scalar;
}

/**
 * @brief Predefined vector constants
 */
namespace VectorConstants {
    static const Vec3 ZERO_3D(0.0f, 0.0f, 0.0f);
    static const Vec3 UP_3D(0.0f, 0.0f, 1.0f);
    static const Vec3 FORWARD_3D(1.0f, 0.0f, 0.0f);
    static const Vec3 RIGHT_3D(0.0f, 1.0f, 0.0f);
    
    static const Vec2 ZERO_2D(0.0f, 0.0f);
    static const Vec2 UP_2D(0.0f, 1.0f);
    static const Vec2 RIGHT_2D(1.0f, 0.0f);
}

/**
 * @brief Additional utility functions (implemented in Vector.cpp)
 */
namespace VectorUtils {
    float AngleBetweenVectors(const Vec3& v1, const Vec3& v2);
    float AngleBetweenVectors(const Vec2& v1, const Vec2& v2);
    Vec3 ProjectVector(const Vec3& a, const Vec3& b);
    Vec2 ProjectVector(const Vec2& a, const Vec2& b);
    Vec3 ReflectVector(const Vec3& incident, const Vec3& normal);
    Vec2 ReflectVector(const Vec2& incident, const Vec2& normal);
    Vec3 BarycentricCoordinates(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c);
    bool IsPointInTriangle(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c);
    Vec3 ClosestPointOnLineSegment(const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd);
    Vec2 ClosestPointOnLineSegment(const Vec2& point, const Vec2& lineStart, const Vec2& lineEnd);
}

namespace Interpolation {
    Vec3 CubicInterpolate(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
    Vec2 CubicInterpolate(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t);
    float SmoothStep(float t);
    float SmootherStep(float t);
    Vec3 BezierQuadratic(const Vec3& p0, const Vec3& p1, const Vec3& p2, float t);
    Vec3 BezierCubic(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t);
}

namespace MatrixOps {
    std::array<std::array<float, 3>, 3> CreateRotationMatrix(const Vec3& axis, float angle);
    Vec3 ApplyRotationMatrix(const std::array<std::array<float, 3>, 3>& matrix, const Vec3& vector);
}

} // namespace VectorMath

// Bring the main classes and functions into global scope for convenience
using VectorMath::Vec2;
using VectorMath::Vec3;
using VectorMath::CalculateAngle;
using VectorMath::CalculateFOV;
using VectorMath::ClampAngles;
using VectorMath::Lerp;
using VectorMath::SlerpAngles;

// Create namespace alias for VectorConstants
namespace VectorConstants = VectorMath::VectorConstants;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
