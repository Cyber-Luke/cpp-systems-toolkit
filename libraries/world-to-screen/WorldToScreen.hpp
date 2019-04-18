/**
 * @file WorldToScreen.hpp
 * @brief 3D to 2D coordinate transformation library
 * @author Lukas Ernst
 * 
 * A comprehensive library for converting 3D world coordinates to 2D screen coordinates.
 * Essential for computer graphics, game development, and visualization applications.
 * Designed for educational purposes and real-time graphics programming.
 * 
 * Features:
 * - World-to-Screen coordinate transformation
 * - View matrix operations
 * - Perspective projection calculations
 * - Screen boundary validation
 * - Optimized matrix operations
 */

#pragma once

#include "../vector-math/Vector.hpp"
#include <array>
#include <iostream>
#include <iomanip>
#include <cmath>

#ifndef DEG2RAD
#define DEG2RAD(degrees) ((degrees) * M_PI / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG(radians) ((radians) * 180.0f / M_PI)
#endif

/**
 * @brief 4x4 Matrix for 3D transformations
 */
struct Matrix4x4 {
    float m[4][4];

    Matrix4x4() {
        // Initialize to identity matrix
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    Matrix4x4(const std::array<std::array<float, 4>, 4>& matrix) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = matrix[i][j];
            }
        }
    }

    // Matrix multiplication
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Vector transformation
    Vec3 TransformVector(const Vec3& vector) const {
        return Vec3(
            m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z + m[0][3],
            m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z + m[1][3],
            m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z + m[2][3]
        );
    }

    // Get the W component after transformation
    float GetTransformW(const Vec3& vector) const {
        return m[3][0] * vector.x + m[3][1] * vector.y + m[3][2] * vector.z + m[3][3];
    }

    // Transform point (returns Vec3)
    Vec3 TransformPoint(const Vec3& point) const {
        float w = GetTransformW(point);
        if (w != 0.0f) {
            Vec3 result = TransformVector(point);
            return Vec3(result.x / w, result.y / w, result.z / w);
        }
        return TransformVector(point);
    }

    // Print matrix for debugging
    void Print() const {
        for (int i = 0; i < 4; ++i) {
            std::cout << "[";
            for (int j = 0; j < 4; ++j) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(3) << m[i][j];
                if (j < 3) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }

    // Static factory methods
    static Matrix4x4 Identity() {
        Matrix4x4 result;
        // Already initialized as identity in constructor
        return result;
    }

    static Matrix4x4 CreatePerspective(float fov, float aspect, float nearPlane, float farPlane) {
        Matrix4x4 result;
        float tanHalfFovy = tan(fov * 0.5f);
        
        result.m[0][0] = 1.0f / (aspect * tanHalfFovy);
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = 1.0f / tanHalfFovy;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.m[2][3] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;

        return result;
    }

    static Matrix4x4 CreateLookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 forward = (center - eye).Normalized();
        Vec3 right = forward.Cross(up).Normalized();
        Vec3 newUp = right.Cross(forward);

        Matrix4x4 result;
        result.m[0][0] = right.x;
        result.m[0][1] = newUp.x;
        result.m[0][2] = -forward.x;
        result.m[0][3] = 0.0f;

        result.m[1][0] = right.y;
        result.m[1][1] = newUp.y;
        result.m[1][2] = -forward.y;
        result.m[1][3] = 0.0f;

        result.m[2][0] = right.z;
        result.m[2][1] = newUp.z;
        result.m[2][2] = -forward.z;
        result.m[2][3] = 0.0f;

        result.m[3][0] = -right.Dot(eye);
        result.m[3][1] = -newUp.Dot(eye);
        result.m[3][2] = forward.Dot(eye);
        result.m[3][3] = 1.0f;

        return result;
    }

    static Matrix4x4 CreateOrthographic(float width, float height, float nearPlane, float farPlane) {
        Matrix4x4 result;
        
        result.m[0][0] = 2.0f / width;
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = 2.0f / height;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = -2.0f / (farPlane - nearPlane);
        result.m[2][3] = -(farPlane + nearPlane) / (farPlane - nearPlane);

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    static Matrix4x4 CreateTranslation(const Vec3& translation) {
        Matrix4x4 result;
        result.m[0][3] = translation.x;
        result.m[1][3] = translation.y;
        result.m[2][3] = translation.z;
        return result;
    }

    static Matrix4x4 CreateRotationY(float degrees) {
        float radians = DEG2RAD(degrees);
        float cosA = cos(radians);
        float sinA = sin(radians);

        Matrix4x4 result;
        result.m[0][0] = cosA;
        result.m[0][2] = sinA;
        result.m[2][0] = -sinA;
        result.m[2][2] = cosA;
        return result;
    }

    static Matrix4x4 CreateScale(const Vec3& scale) {
        Matrix4x4 result;
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;
        return result;
    }
};

/**
 * @brief Screen dimensions and viewport information
 */
struct Viewport {
    int width;
    int height;
    float x_offset;
    float y_offset;

    Viewport() : width(800), height(600), x_offset(0.0f), y_offset(0.0f) {}

    Viewport(int w, int h, float x_off = 0.0f, float y_off = 0.0f) 
        : width(w), height(h), x_offset(x_off), y_offset(y_off) {}

    void SetViewport(int w, int h, float x_off = 0.0f, float y_off = 0.0f) {
        width = w;
        height = h;
        x_offset = x_off;
        y_offset = y_off;
    }

    Vec2 GetCenter() const {
        return Vec2(width * 0.5f + x_offset, height * 0.5f + y_offset);
    }

    bool IsPointInside(const Vec2& point) const {
        return point.x >= x_offset && point.x < (width + x_offset) &&
               point.y >= y_offset && point.y < (height + y_offset);
    }
};

/**
 * @brief Main class for world-to-screen transformations
 */
class WorldToScreenTransform {
private:
    Matrix4x4 m_viewMatrix;
    Viewport m_viewport;
    bool m_matrixValid;

public:
    WorldToScreenTransform(const Viewport& viewport) 
        : m_viewport(viewport), m_matrixValid(false) {}

    /**
     * @brief Sets the view matrix for transformations
     * @param matrix The 4x4 view matrix
     */
    void SetViewMatrix(const Matrix4x4& matrix) {
        m_viewMatrix = matrix;
        m_matrixValid = true;
    }

    /**
     * @brief Updates the viewport dimensions
     * @param viewport New viewport settings
     */
    void SetViewport(const Viewport& viewport) {
        m_viewport = viewport;
    }

    /**
     * @brief Transforms a 3D world position to 2D screen coordinates
     * @param worldPos 3D position in world space
     * @param screenPos Output 2D screen position
     * @return true if the point is visible (in front of camera), false if behind
     */
    bool WorldToScreen(const Vec3& worldPos, Vec2& screenPos) const {
        if (!m_matrixValid) {
            return false;
        }

        // Transform the point through the view matrix
        Vec3 transformed = m_viewMatrix.TransformVector(worldPos);
        float w = m_viewMatrix.GetTransformW(worldPos);

        // Check if point is behind the camera
        if (w < 0.001f) {
            return false;
        }

        // Perspective divide
        float invW = 1.0f / w;
        transformed.x *= invW;
        transformed.y *= invW;

        // Convert to screen coordinates
        Vec2 center = m_viewport.GetCenter();
        screenPos.x = center.x + (transformed.x * m_viewport.width * 0.5f);
        screenPos.y = center.y - (transformed.y * m_viewport.height * 0.5f);

        return true;
    }

    /**
     * @brief Transforms multiple points at once for better performance
     * @param worldPoints Array of 3D world positions
     * @param screenPoints Output array of 2D screen positions
     * @param count Number of points to transform
     * @return Number of successfully transformed points
     */
    int WorldToScreenBatch(const Vec3* worldPoints, Vec2* screenPoints, int count) const {
        if (!m_matrixValid) {
            return 0;
        }

        int successCount = 0;
        Vec2 center = m_viewport.GetCenter();
        
        for (int i = 0; i < count; ++i) {
            Vec3 transformed = m_viewMatrix.TransformVector(worldPoints[i]);
            float w = m_viewMatrix.GetTransformW(worldPoints[i]);

            if (w >= 0.001f) {
                float invW = 1.0f / w;
                transformed.x *= invW;
                transformed.y *= invW;

                screenPoints[i].x = center.x + (transformed.x * m_viewport.width * 0.5f);
                screenPoints[i].y = center.y - (transformed.y * m_viewport.height * 0.5f);
                
                successCount++;
            } else {
                // Mark invalid points
                screenPoints[i] = Vec2(-1.0f, -1.0f);
            }
        }

        return successCount;
    }

    /**
     * @brief Checks if a 3D point would be visible on screen
     * @param worldPos 3D position in world space
     * @return true if the point would be visible
     */
    bool IsPointVisible(const Vec3& worldPos) const {
        Vec2 screenPos;
        if (!WorldToScreen(worldPos, screenPos)) {
            return false;
        }
        return m_viewport.IsPointInside(screenPos);
    }

    /**
     * @brief Calculates the distance from camera to a 3D point
     * @param worldPos 3D position in world space
     * @return Distance in world units, or -1 if behind camera
     */
    float GetDistanceToPoint(const Vec3& worldPos) const {
        if (!m_matrixValid) {
            return -1.0f;
        }

        float w = m_viewMatrix.GetTransformW(worldPos);
        return (w >= 0.001f) ? w : -1.0f;
    }

    /**
     * @brief Gets the current viewport
     * @return Current viewport settings
     */
    const Viewport& GetViewport() const { return m_viewport; }

    /**
     * @brief Checks if the view matrix is valid
     * @return true if matrix is set and valid
     */
    bool IsMatrixValid() const { return m_matrixValid; }
};

/**
 * @brief Utility functions for common transformations
 */
namespace W2SUtils {
    /**
     * @brief Creates a perspective projection matrix
     * @param fovY Field of view in Y direction (radians)
     * @param aspectRatio Width/Height ratio
     * @param nearPlane Near clipping plane
     * @param farPlane Far clipping plane
     * @return Perspective projection matrix
     */
    Matrix4x4 CreatePerspectiveMatrix(float fovY, float aspectRatio, float nearPlane, float farPlane);

    /**
     * @brief Creates a look-at view matrix
     * @param eye Camera position
     * @param target Look-at target
     * @param up Up vector
     * @return View matrix
     */
    Matrix4x4 CreateLookAtMatrix(const Vec3& eye, const Vec3& target, const Vec3& up);

    /**
     * @brief Quick world-to-screen transformation without class overhead
     * @param worldPos 3D world position
     * @param viewMatrix View transformation matrix
     * @param viewport Screen viewport
     * @param screenPos Output 2D screen position
     * @return true if transformation successful
     */
    bool QuickWorldToScreen(const Vec3& worldPos, const Matrix4x4& viewMatrix, 
                           const Viewport& viewport, Vec2& screenPos);

    /**
     * @brief Create a view matrix from position and Euler angles
     */
    Matrix4x4 CreateViewMatrixFromEuler(const Vec3& position, float pitch, float yaw, float roll);

    /**
     * @brief Create an orthographic projection matrix
     */
    Matrix4x4 CreateOrthographicMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    /**
     * @brief Calculate field of view from projection matrix
     */
    float ExtractFOVFromProjectionMatrix(const Matrix4x4& projMatrix);

    /**
     * @brief Extract camera position from view matrix
     */
    Vec3 ExtractCameraPositionFromViewMatrix(const Matrix4x4& viewMatrix);

    /**
     * @brief Ray structure for screen-to-world conversion
     */
    struct Ray {
        Vec3 origin;
        Vec3 direction;
        
        Ray() = default;
        Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
    };

    /**
     * @brief Convert screen coordinates back to world ray
     */
    Ray ScreenToWorldRay(const Vec2& screenPos, const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix, const Viewport& viewport);

    /**
     * @brief Calculate matrix inverse
     */
    Matrix4x4 InverseMatrix(const Matrix4x4& matrix);

    /**
     * @brief Check if a 3D bounding box is visible in the view frustum
     */
    bool IsBoundingBoxVisible(const Vec3& minBounds, const Vec3& maxBounds, const Matrix4x4& viewProjMatrix, const Viewport& viewport);

    /**
     * @brief Screen rectangle structure
     */
    struct ScreenRect {
        float left, right, top, bottom;
        bool valid;
        
        ScreenRect() : left(0), right(0), top(0), bottom(0), valid(false) {}
        ScreenRect(float l, float r, float t, float b) : left(l), right(r), top(t), bottom(b), valid(true) {}
        
        float Width() const { return right - left; }
        float Height() const { return bottom - top; }
        Vec2 Center() const { return Vec2((left + right) * 0.5f, (top + bottom) * 0.5f); }
    };

    /**
     * @brief Calculate the screen-space bounding rectangle of a 3D bounding box
     */
    ScreenRect GetScreenBounds(const Vec3& minBounds, const Vec3& maxBounds, const Matrix4x4& viewMatrix, const Viewport& viewport);
}
