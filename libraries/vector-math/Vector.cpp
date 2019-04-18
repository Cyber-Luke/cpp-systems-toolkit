/**
 * @file Vector.cpp
 * @brief Implementation of advanced vector mathematics operations
 * @author Lukas Ernst
 * 
 * Extended implementations for vector mathematics library including advanced
 * geometric calculations, interpolation functions, and utility operations.
 * Provides additional functionality beyond the inline header implementations.
 */

#include "Vector.hpp"
#include <array>
#include <cmath>

// Vector.cpp - Additional implementations for VectorMath library
// Most functions are implemented inline in the header for performance,
// but we can add some extended functionality here.

namespace VectorMath {

/**
 * @brief Additional utility functions for vector operations
 */
namespace VectorUtils {

/**
 * @brief Calculate the angle between two 3D vectors in radians
 */
float AngleBetweenVectors(const Vec3& v1, const Vec3& v2) {
    float dot = v1.Dot(v2);
    float len1 = v1.Length();
    float len2 = v2.Length();
    
    if (len1 < 1e-6f || len2 < 1e-6f) {
        return 0.0f;
    }
    
    float cosAngle = dot / (len1 * len2);
    // Clamp to avoid floating point errors with acos
    cosAngle = std::max(-1.0f, std::min(1.0f, cosAngle));
    
    return std::acos(cosAngle);
}

/**
 * @brief Calculate the angle between two 2D vectors in radians
 */
float AngleBetweenVectors(const Vec2& v1, const Vec2& v2) {
    float dot = v1.Dot(v2);
    float len1 = v1.Length();
    float len2 = v2.Length();
    
    if (len1 < 1e-6f || len2 < 1e-6f) {
        return 0.0f;
    }
    
    float cosAngle = dot / (len1 * len2);
    // Clamp to avoid floating point errors with acos
    cosAngle = std::max(-1.0f, std::min(1.0f, cosAngle));
    
    return std::acos(cosAngle);
}

/**
 * @brief Project vector a onto vector b
 */
Vec3 ProjectVector(const Vec3& a, const Vec3& b) {
    float b_length_squared = b.LengthSquared();
    if (b_length_squared < 1e-6f) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    
    float projection_length = a.Dot(b) / b_length_squared;
    return b * projection_length;
}

/**
 * @brief Project vector a onto vector b (2D)
 */
Vec2 ProjectVector(const Vec2& a, const Vec2& b) {
    float b_length_squared = b.LengthSquared();
    if (b_length_squared < 1e-6f) {
        return Vec2(0.0f, 0.0f);
    }
    
    float projection_length = a.Dot(b) / b_length_squared;
    return b * projection_length;
}

/**
 * @brief Reflect vector a across vector b (3D)
 */
Vec3 ReflectVector(const Vec3& incident, const Vec3& normal) {
    Vec3 norm = normal.Normalized();
    return incident - norm * (2.0f * incident.Dot(norm));
}

/**
 * @brief Reflect vector a across vector b (2D)
 */
Vec2 ReflectVector(const Vec2& incident, const Vec2& normal) {
    Vec2 norm = normal;
    norm.Normalize();
    return incident - norm * (2.0f * incident.Dot(norm));
}

/**
 * @brief Calculate barycentric coordinates of point p in triangle (a, b, c)
 */
Vec3 BarycentricCoordinates(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
    Vec2 v0 = c - a;
    Vec2 v1 = b - a;
    Vec2 v2 = p - a;
    
    float dot00 = v0.Dot(v0);
    float dot01 = v0.Dot(v1);
    float dot02 = v0.Dot(v2);
    float dot11 = v1.Dot(v1);
    float dot12 = v1.Dot(v2);
    
    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    return Vec3(1.0f - u - v, v, u); // (alpha, beta, gamma)
}

/**
 * @brief Check if point is inside triangle using barycentric coordinates
 */
bool IsPointInTriangle(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
    Vec3 bary = BarycentricCoordinates(p, a, b, c);
    return (bary.x >= 0.0f) && (bary.y >= 0.0f) && (bary.z >= 0.0f);
}

/**
 * @brief Calculate the closest point on a line segment to a given point
 */
Vec3 ClosestPointOnLineSegment(const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd) {
    Vec3 line = lineEnd - lineStart;
    float lineLength = line.LengthSquared();
    
    if (lineLength < 1e-6f) {
        return lineStart; // Degenerate case: line is a point
    }
    
    float t = (point - lineStart).Dot(line) / lineLength;
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp to [0,1]
    
    return lineStart + line * t;
}

/**
 * @brief Calculate the closest point on a line segment to a given point (2D)
 */
Vec2 ClosestPointOnLineSegment(const Vec2& point, const Vec2& lineStart, const Vec2& lineEnd) {
    Vec2 line = lineEnd - lineStart;
    float lineLength = line.LengthSquared();
    
    if (lineLength < 1e-6f) {
        return lineStart; // Degenerate case: line is a point
    }
    
    float t = (point - lineStart).Dot(line) / lineLength;
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp to [0,1]
    
    return lineStart + line * t;
}

} // namespace VectorUtils

/**
 * @brief Advanced interpolation functions
 */
namespace Interpolation {

/**
 * @brief Cubic interpolation between four points
 */
Vec3 CubicInterpolate(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    
    Vec3 a = p3 - p2 - p0 + p1;
    Vec3 b = p0 - p1 - a;
    Vec3 c = p2 - p0;
    Vec3 d = p1;
    
    return a * t3 + b * t2 + c * t + d;
}

/**
 * @brief Cubic interpolation between four points (2D)
 */
Vec2 CubicInterpolate(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    
    Vec2 a = p3 - p2 - p0 + p1;
    Vec2 b = p0 - p1 - a;
    Vec2 c = p2 - p0;
    Vec2 d = p1;
    
    return a * t3 + b * t2 + c * t + d;
}

/**
 * @brief Smooth step interpolation (3rd order polynomial)
 */
float SmoothStep(float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    return t * t * (3.0f - 2.0f * t);
}

/**
 * @brief Smoother step interpolation (5th order polynomial)
 */
float SmootherStep(float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/**
 * @brief Bezier curve interpolation (quadratic)
 */
Vec3 BezierQuadratic(const Vec3& p0, const Vec3& p1, const Vec3& p2, float t) {
    float u = 1.0f - t;
    return p0 * (u * u) + p1 * (2.0f * u * t) + p2 * (t * t);
}

/**
 * @brief Bezier curve interpolation (cubic)
 */
Vec3 BezierCubic(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float u = 1.0f - t;
    float u2 = u * u;
    float u3 = u2 * u;
    float t2 = t * t;
    float t3 = t2 * t;
    
    return p0 * u3 + p1 * (3.0f * u2 * t) + p2 * (3.0f * u * t2) + p3 * t3;
}

} // namespace Interpolation

/**
 * @brief Matrix helper functions
 */
namespace MatrixOps {

/**
 * @brief Create rotation matrix for rotating around arbitrary axis
 */
std::array<std::array<float, 3>, 3> CreateRotationMatrix(const Vec3& axis, float angle) {
    Vec3 a = axis.Normalized();
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    float one_minus_cos = 1.0f - cos_a;
    
    std::array<std::array<float, 3>, 3> result;
    
    result[0][0] = cos_a + a.x * a.x * one_minus_cos;
    result[0][1] = a.x * a.y * one_minus_cos - a.z * sin_a;
    result[0][2] = a.x * a.z * one_minus_cos + a.y * sin_a;
    
    result[1][0] = a.y * a.x * one_minus_cos + a.z * sin_a;
    result[1][1] = cos_a + a.y * a.y * one_minus_cos;
    result[1][2] = a.y * a.z * one_minus_cos - a.x * sin_a;
    
    result[2][0] = a.z * a.x * one_minus_cos - a.y * sin_a;
    result[2][1] = a.z * a.y * one_minus_cos + a.x * sin_a;
    result[2][2] = cos_a + a.z * a.z * one_minus_cos;
    
    return result;
}

/**
 * @brief Apply rotation matrix to vector
 */
Vec3 ApplyRotationMatrix(const std::array<std::array<float, 3>, 3>& matrix, const Vec3& vector) {
    return Vec3(
        matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z,
        matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * vector.z,
        matrix[2][0] * vector.x + matrix[2][1] * vector.y + matrix[2][2] * vector.z
    );
}

} // namespace MatrixOps

} // namespace VectorMath

// Global functions moved from header
namespace VectorMath {

/**
 * @brief Calculate angle between two 3D points
 */
Vec2 CalculateAngle(const Vec3& from, const Vec3& to) {
    Vec3 delta = to - from;
    float distance = delta.Length2D();
    
    Vec2 angle;
    angle.y = std::atan2(delta.y, delta.x) * 180.0f / static_cast<float>(M_PI); // Yaw
    angle.x = std::atan2(-delta.z, distance) * 180.0f / static_cast<float>(M_PI); // Pitch
    
    return angle;
}

/**
 * @brief Calculate field of view between two angles
 */
float CalculateFOV(const Vec2& current, const Vec2& target) {
    float deltaX = target.x - current.x;
    float deltaY = target.y - current.y;
    
    // Normalize angle differences
    while (deltaY > 180.0f) deltaY -= 360.0f;
    while (deltaY < -180.0f) deltaY += 360.0f;
    
    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

/**
 * @brief Clamp angles to valid ranges
 */
void ClampAngles(float angles[2]) {
    // Clamp pitch to [-89, 89]
    angles[0] = std::max(-89.0f, std::min(89.0f, angles[0]));
    
    // Normalize yaw to [0, 360)
    while (angles[1] >= 360.0f) angles[1] -= 360.0f;
    while (angles[1] < 0.0f) angles[1] += 360.0f;
}

/**
 * @brief Linear interpolation between two 3D vectors
 */
Vec3 Lerp(const Vec3& start, const Vec3& end, float t) {
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp t to [0,1]
    return Vec3(
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.z + (end.z - start.z) * t
    );
}

/**
 * @brief Angular interpolation with proper wrapping
 */
Vec2 SlerpAngles(const Vec2& start, const Vec2& end, float t) {
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp t to [0,1]
    
    Vec2 result;
    result.x = start.x + (end.x - start.x) * t;
    
    // Handle yaw wraparound
    float yawDiff = end.y - start.y;
    while (yawDiff > 180.0f) yawDiff -= 360.0f;
    while (yawDiff < -180.0f) yawDiff += 360.0f;
    
    result.y = start.y + yawDiff * t;
    
    // Normalize result
    while (result.y >= 360.0f) result.y -= 360.0f;
    while (result.y < 0.0f) result.y += 360.0f;
    
    return result;
}

} // namespace VectorMath