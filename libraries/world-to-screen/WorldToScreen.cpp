/**
 * @file WorldToScreen.cpp
 * @brief Implementation of 3D to 2D coordinate transformation library
 * @author Lukas Ernst
 * 
 * Implementation of world-to-screen coordinate transformations including matrix
 * operations, perspective projections, and utility functions. Provides optimized
 * algorithms for real-time graphics and educational applications.
 */

#include "WorldToScreen.hpp"
#include <cfloat>
#include <algorithm>

#ifndef DEG2RAD
#define DEG2RAD(degrees) ((degrees) * M_PI / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG(radians) ((radians) * 180.0f / M_PI)
#endif

// WorldToScreen.cpp - Additional implementations and utilities

namespace W2SUtils {

/**
 * @brief Creates a perspective projection matrix
 */
Matrix4x4 CreatePerspectiveMatrix(float fovY, float aspectRatio, float nearPlane, float farPlane) {
    Matrix4x4 matrix;
    
    float tanHalfFov = std::tan(fovY * 0.5f);
    
    matrix.m[0][0] = 1.0f / (aspectRatio * tanHalfFov);
    matrix.m[1][1] = 1.0f / tanHalfFov;
    matrix.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    matrix.m[2][3] = -1.0f;
    matrix.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    matrix.m[3][3] = 0.0f;
    
    return matrix;
}

/**
 * @brief Creates a look-at view matrix
 */
Matrix4x4 CreateLookAtMatrix(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 zAxis = (eye - target);
    zAxis.Normalize();
    
    Vec3 xAxis = up.Cross(zAxis);
    xAxis.Normalize();
    
    Vec3 yAxis = zAxis.Cross(xAxis);
    
    Matrix4x4 matrix;
    matrix.m[0][0] = xAxis.x;
    matrix.m[1][0] = xAxis.y;
    matrix.m[2][0] = xAxis.z;
    matrix.m[3][0] = -xAxis.Dot(eye);
    
    matrix.m[0][1] = yAxis.x;
    matrix.m[1][1] = yAxis.y;
    matrix.m[2][1] = yAxis.z;
    matrix.m[3][1] = -yAxis.Dot(eye);
    
    matrix.m[0][2] = zAxis.x;
    matrix.m[1][2] = zAxis.y;
    matrix.m[2][2] = zAxis.z;
    matrix.m[3][2] = -zAxis.Dot(eye);
    
    matrix.m[0][3] = 0.0f;
    matrix.m[1][3] = 0.0f;
    matrix.m[2][3] = 0.0f;
    matrix.m[3][3] = 1.0f;
    
    return matrix;
}

/**
 * @brief Quick world-to-screen transformation without class overhead
 */
bool QuickWorldToScreen(const Vec3& worldPos, const Matrix4x4& viewMatrix, 
                       const Viewport& viewport, Vec2& screenPos) {
    Vec3 transformed = viewMatrix.TransformVector(worldPos);
    float w = viewMatrix.GetTransformW(worldPos);

    if (w < 0.001f) {
        return false;
    }

    float invW = 1.0f / w;
    transformed.x *= invW;
    transformed.y *= invW;

    Vec2 center = viewport.GetCenter();
    screenPos.x = center.x + (transformed.x * viewport.width * 0.5f);
    screenPos.y = center.y - (transformed.y * viewport.height * 0.5f);

    return true;
}

/**
 * @brief Create a view matrix from position and Euler angles
 */
Matrix4x4 CreateViewMatrixFromEuler(const Vec3& position, float pitch, float yaw, float roll) {
    // Convert degrees to radians
    float pitchRad = DEG2RAD(pitch);
    float yawRad = DEG2RAD(yaw);
    float rollRad = DEG2RAD(roll);
    
    // Calculate rotation matrices
    float cosPitch = std::cos(pitchRad);
    float sinPitch = std::sin(pitchRad);
    float cosYaw = std::cos(yawRad);
    float sinYaw = std::sin(yawRad);
    float cosRoll = std::cos(rollRad);
    float sinRoll = std::sin(rollRad);
    
    // Combined rotation matrix (YXZ order)
    Matrix4x4 rotation;
    
    rotation.m[0][0] = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
    rotation.m[0][1] = cosRoll * sinYaw * sinPitch - cosYaw * sinRoll;
    rotation.m[0][2] = cosPitch * sinYaw;
    rotation.m[0][3] = 0.0f;
    
    rotation.m[1][0] = cosPitch * sinRoll;
    rotation.m[1][1] = cosPitch * cosRoll;
    rotation.m[1][2] = -sinPitch;
    rotation.m[1][3] = 0.0f;
    
    rotation.m[2][0] = cosYaw * sinPitch * sinRoll - cosRoll * sinYaw;
    rotation.m[2][1] = cosYaw * cosRoll * sinPitch + sinYaw * sinRoll;
    rotation.m[2][2] = cosYaw * cosPitch;
    rotation.m[2][3] = 0.0f;
    
    rotation.m[3][0] = 0.0f;
    rotation.m[3][1] = 0.0f;
    rotation.m[3][2] = 0.0f;
    rotation.m[3][3] = 1.0f;
    
    // Create translation matrix
    Matrix4x4 translation = Matrix4x4::CreateTranslation(Vec3(-position.x, -position.y, -position.z));
    
    // Combine rotation and translation (rotation * translation)
    return rotation * translation;
}

/**
 * @brief Create an orthographic projection matrix
 */
Matrix4x4 CreateOrthographicMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    Matrix4x4 matrix;
    
    matrix.m[0][0] = 2.0f / (right - left);
    matrix.m[0][1] = 0.0f;
    matrix.m[0][2] = 0.0f;
    matrix.m[0][3] = -(right + left) / (right - left);
    
    matrix.m[1][0] = 0.0f;
    matrix.m[1][1] = 2.0f / (top - bottom);
    matrix.m[1][2] = 0.0f;
    matrix.m[1][3] = -(top + bottom) / (top - bottom);
    
    matrix.m[2][0] = 0.0f;
    matrix.m[2][1] = 0.0f;
    matrix.m[2][2] = -2.0f / (farPlane - nearPlane);
    matrix.m[2][3] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    
    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.0f;
    
    return matrix;
}

/**
 * @brief Calculate field of view from projection matrix
 */
float ExtractFOVFromProjectionMatrix(const Matrix4x4& projMatrix) {
    // Extract FOV from perspective projection matrix
    // FOV = 2 * atan(1 / m[1][1])
    if (projMatrix.m[1][1] != 0.0f) {
        return 2.0f * std::atan(1.0f / projMatrix.m[1][1]);
    }
    return 0.0f;
}

/**
 * @brief Extract camera position from view matrix
 */
Vec3 ExtractCameraPositionFromViewMatrix(const Matrix4x4& viewMatrix) {
    // For a view matrix, the camera position can be extracted using:
    // position = -(R^T * t) where R is the rotation part and t is the translation part
    
    // Extract the rotation part (upper-left 3x3)
    float r00 = viewMatrix.m[0][0], r01 = viewMatrix.m[1][0], r02 = viewMatrix.m[2][0];
    float r10 = viewMatrix.m[0][1], r11 = viewMatrix.m[1][1], r12 = viewMatrix.m[2][1];
    float r20 = viewMatrix.m[0][2], r21 = viewMatrix.m[1][2], r22 = viewMatrix.m[2][2];
    
    // Extract the translation part
    float tx = viewMatrix.m[3][0];
    float ty = viewMatrix.m[3][1];
    float tz = viewMatrix.m[3][2];
    
    // Calculate camera position: -(R^T * t)
    Vec3 position;
    position.x = -(r00 * tx + r01 * ty + r02 * tz);
    position.y = -(r10 * tx + r11 * ty + r12 * tz);
    position.z = -(r20 * tx + r21 * ty + r22 * tz);
    
    return position;
}

/**
 * @brief Convert screen coordinates back to world ray
 */

Ray ScreenToWorldRay(const Vec2& screenPos, const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix, const Viewport& viewport) {
    // Convert screen coordinates to normalized device coordinates (NDC)
    float ndcX = (2.0f * (screenPos.x - viewport.x_offset)) / viewport.width - 1.0f;
    float ndcY = 1.0f - (2.0f * (screenPos.y - viewport.y_offset)) / viewport.height;
    
    // Create points in clip space
    Vec3 nearPoint(ndcX, ndcY, -1.0f); // Near plane
    Vec3 farPoint(ndcX, ndcY, 1.0f);   // Far plane
    
    // Calculate inverse matrices
    Matrix4x4 invView = InverseMatrix(viewMatrix);
    Matrix4x4 invProj = InverseMatrix(projMatrix);
    
    // Transform to world space
    Vec3 nearWorld = invView.TransformVector(invProj.TransformVector(nearPoint));
    Vec3 farWorld = invView.TransformVector(invProj.TransformVector(farPoint));
    
    // Create ray
    Vec3 rayOrigin = nearWorld;
    Vec3 rayDirection = (farWorld - nearWorld);
    rayDirection.Normalize();
    
    return Ray(rayOrigin, rayDirection);
}

/**
 * @brief Calculate matrix inverse (simplified for 4x4 matrices)
 */
Matrix4x4 InverseMatrix(const Matrix4x4& matrix) {
    Matrix4x4 result;
    
    // This is a simplified implementation. For a complete implementation,
    // you would use the full 4x4 matrix inversion algorithm.
    // For now, we'll assume it's an affine transformation matrix.
    
    // Extract the 3x3 rotation part
    float r00 = matrix.m[0][0], r01 = matrix.m[0][1], r02 = matrix.m[0][2];
    float r10 = matrix.m[1][0], r11 = matrix.m[1][1], r12 = matrix.m[1][2];
    float r20 = matrix.m[2][0], r21 = matrix.m[2][1], r22 = matrix.m[2][2];
    
    // Calculate determinant of 3x3 part
    float det = r00 * (r11 * r22 - r12 * r21) - 
                r01 * (r10 * r22 - r12 * r20) + 
                r02 * (r10 * r21 - r11 * r20);
    
    if (std::abs(det) < 1e-6f) {
        return Matrix4x4(); // Return identity if not invertible
    }
    
    float invDet = 1.0f / det;
    
    // Calculate inverse of rotation part (transpose for orthogonal matrices)
    result.m[0][0] = (r11 * r22 - r12 * r21) * invDet;
    result.m[0][1] = (r02 * r21 - r01 * r22) * invDet;
    result.m[0][2] = (r01 * r12 - r02 * r11) * invDet;
    result.m[0][3] = 0.0f;
    
    result.m[1][0] = (r12 * r20 - r10 * r22) * invDet;
    result.m[1][1] = (r00 * r22 - r02 * r20) * invDet;
    result.m[1][2] = (r02 * r10 - r00 * r12) * invDet;
    result.m[1][3] = 0.0f;
    
    result.m[2][0] = (r10 * r21 - r11 * r20) * invDet;
    result.m[2][1] = (r01 * r20 - r00 * r21) * invDet;
    result.m[2][2] = (r00 * r11 - r01 * r10) * invDet;
    result.m[2][3] = 0.0f;
    
    // Calculate inverse translation
    float tx = matrix.m[3][0];
    float ty = matrix.m[3][1];
    float tz = matrix.m[3][2];
    
    result.m[3][0] = -(result.m[0][0] * tx + result.m[1][0] * ty + result.m[2][0] * tz);
    result.m[3][1] = -(result.m[0][1] * tx + result.m[1][1] * ty + result.m[2][1] * tz);
    result.m[3][2] = -(result.m[0][2] * tx + result.m[1][2] * ty + result.m[2][2] * tz);
    result.m[3][3] = 1.0f;
    
    return result;
}

/**
 * @brief Check if a 3D bounding box is visible in the view frustum
 */
bool IsBoundingBoxVisible(const Vec3& minBounds, const Vec3& maxBounds, const Matrix4x4& viewProjMatrix, const Viewport& viewport) {
    // Test all 8 corners of the bounding box
    Vec3 corners[8] = {
        Vec3(minBounds.x, minBounds.y, minBounds.z),
        Vec3(maxBounds.x, minBounds.y, minBounds.z),
        Vec3(minBounds.x, maxBounds.y, minBounds.z),
        Vec3(maxBounds.x, maxBounds.y, minBounds.z),
        Vec3(minBounds.x, minBounds.y, maxBounds.z),
        Vec3(maxBounds.x, minBounds.y, maxBounds.z),
        Vec3(minBounds.x, maxBounds.y, maxBounds.z),
        Vec3(maxBounds.x, maxBounds.y, maxBounds.z)
    };
    
    int visibleCorners = 0;
    for (int i = 0; i < 8; ++i) {
        Vec2 screenPos;
        WorldToScreenTransform transform(viewport);
        transform.SetViewMatrix(viewProjMatrix);
        
        if (transform.WorldToScreen(corners[i], screenPos) && viewport.IsPointInside(screenPos)) {
            visibleCorners++;
        }
    }
    
    // If any corner is visible, the bounding box might be visible
    return visibleCorners > 0;
}

/**
 * @brief Calculate the screen-space bounding rectangle of a 3D bounding box
 */


ScreenRect GetScreenBounds(const Vec3& minBounds, const Vec3& maxBounds, const Matrix4x4& viewMatrix, const Viewport& viewport) {
    Vec3 corners[8] = {
        Vec3(minBounds.x, minBounds.y, minBounds.z),
        Vec3(maxBounds.x, minBounds.y, minBounds.z),
        Vec3(minBounds.x, maxBounds.y, minBounds.z),
        Vec3(maxBounds.x, maxBounds.y, minBounds.z),
        Vec3(minBounds.x, minBounds.y, maxBounds.z),
        Vec3(maxBounds.x, minBounds.y, maxBounds.z),
        Vec3(minBounds.x, maxBounds.y, maxBounds.z),
        Vec3(maxBounds.x, maxBounds.y, maxBounds.z)
    };
    
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;
    int validPoints = 0;
    
    WorldToScreenTransform transform(viewport);
    transform.SetViewMatrix(viewMatrix);
    
    for (int i = 0; i < 8; ++i) {
        Vec2 screenPos;
        if (transform.WorldToScreen(corners[i], screenPos)) {
            minX = std::min(minX, screenPos.x);
            maxX = std::max(maxX, screenPos.x);
            minY = std::min(minY, screenPos.y);
            maxY = std::max(maxY, screenPos.y);
            validPoints++;
        }
    }
    
    if (validPoints > 0) {
        return ScreenRect(minX, maxX, minY, maxY);
    }
    
    return ScreenRect(); // Invalid
}

} // namespace W2SUtils