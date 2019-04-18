# 🌍 World-to-Screen Transformation Library

A high-performance, comprehensive C++ library for converting 3D world coordinates to 2D screen coordinates. Essential for computer graphics, game development, augmented reality, visualization, and user interface overlay systems.

## ✨ Features

### 🎯 Core Transformation Engine
- **3D to 2D Coordinate Transformation**: Accurate world-to-screen projection
- **Perspective & Orthographic Projection**: Support for both projection types
- **Batch Processing**: Optimize multiple point transformations
- **View Matrix Integration**: Flexible camera system support
- **Clipping & Culling**: Automatic behind-camera detection

### 🏗️ Matrix Operations
- **4x4 Matrix Support**: Complete homogeneous coordinate system
- **Matrix Multiplication**: Optimized matrix operations
- **Transformation Chains**: Combine multiple transformations
- **Matrix Utilities**: Pre-built projection and view matrices
- **Inverse Operations**: Screen-to-world ray calculations

### 📺 Viewport Management
- **Flexible Screen Dimensions**: Support any resolution
- **Multi-Monitor Support**: Handle different screen configurations
- **Viewport Offsets**: Support for windowed applications
- **Boundary Testing**: Efficient visibility detection
- **Screen Space Utilities**: Rectangle and bounds calculations

### 🎮 Gaming & Graphics Features
- **Real-time Performance**: Optimized for 60+ FPS applications
- **Frustum Culling**: Bounding box visibility testing
- **Distance Calculations**: Camera distance to objects
- **FOV Utilities**: Field of view calculations and extraction
- **Ray Casting**: Screen-to-world ray generation

### 🔧 Advanced Utilities
- **Camera System Integration**: Extract camera position from view matrices
- **Euler Angle Support**: Create view matrices from pitch/yaw/roll
- **Bounding Box Projection**: 3D bounds to 2D screen rectangles
- **Matrix Inversion**: Robust matrix inverse calculations

## 🚀 Quick Start

### Basic World-to-Screen Transformation
```cpp
#include "libraries/world-to-screen/WorldToScreen.hpp"
#include <iostream>

int main() {
    // Setup viewport (screen dimensions)
    Viewport viewport(1920, 1080);  // Full HD resolution

    // Create transformer
    WorldToScreenTransform transformer(viewport);

    // Create a view matrix (camera looking down negative Z-axis)
    Vec3 cameraPos(0.0f, 0.0f, 10.0f);
    Vec3 targetPos(0.0f, 0.0f, 0.0f);
    Vec3 upVector(0.0f, 1.0f, 0.0f);
    
    Matrix4x4 viewMatrix = Matrix4x4::CreateLookAt(cameraPos, targetPos, upVector);
    transformer.SetViewMatrix(viewMatrix);

    // Transform a 3D point to 2D screen coordinates
    Vec3 worldPos(5.0f, 3.0f, 0.0f);
    Vec2 screenPos;

    if (transformer.WorldToScreen(worldPos, screenPos)) {
        std::cout << "Screen position: (" << screenPos.x << ", " << screenPos.y << ")" << std::endl;
        
        // Check if point is visible within viewport
        if (viewport.IsPointInside(screenPos)) {
            std::cout << "Point is visible on screen!" << std::endl;
        } else {
            std::cout << "Point is outside viewport bounds" << std::endl;
        }
    } else {
        std::cout << "Point is behind the camera" << std::endl;
    }

    return 0;
}
```

### Advanced Camera Setup
```cpp
// Create perspective projection matrix
float fovY = DEG2RAD(75.0f);  // 75 degree field of view
float aspectRatio = 16.0f / 9.0f;  // Widescreen aspect ratio
float nearPlane = 0.1f;
float farPlane = 1000.0f;

Matrix4x4 projMatrix = Matrix4x4::CreatePerspective(fovY, aspectRatio, nearPlane, farPlane);

// Create view matrix from camera position and orientation
Vec3 cameraPosition(100.0f, 50.0f, 200.0f);
float pitch = -15.0f;  // Look slightly down
float yaw = 45.0f;     // Look northeast
float roll = 0.0f;     // No camera tilt

Matrix4x4 viewMatrix = W2SUtils::CreateViewMatrixFromEuler(cameraPosition, pitch, yaw, roll);

// Combine view and projection (typical for graphics applications)
Matrix4x4 viewProjMatrix = projMatrix * viewMatrix;

WorldToScreenTransform transformer(Viewport(1920, 1080));
transformer.SetViewMatrix(viewProjMatrix);
```

### Batch Processing for Performance
```cpp
// Transform multiple points at once for better performance
std::vector<Vec3> worldPoints = {
    Vec3(10.0f, 20.0f, 30.0f),
    Vec3(40.0f, 50.0f, 60.0f),
    Vec3(70.0f, 80.0f, 90.0f),
    Vec3(100.0f, 110.0f, 120.0f),
    Vec3(130.0f, 140.0f, 150.0f)
};

std::vector<Vec2> screenPoints(worldPoints.size());

int successCount = transformer.WorldToScreenBatch(
    worldPoints.data(), 
    screenPoints.data(), 
    worldPoints.size()
);

std::cout << "Successfully transformed " << successCount << " out of " 
          << worldPoints.size() << " points" << std::endl;

// Process results
for (size_t i = 0; i < worldPoints.size(); ++i) {
    if (screenPoints[i].x >= 0 && screenPoints[i].y >= 0) {
        std::cout << "Point " << i << ": (" << screenPoints[i].x 
                  << ", " << screenPoints[i].y << ")" << std::endl;
    } else {
        std::cout << "Point " << i << ": Behind camera or invalid" << std::endl;
    }
}
```

std::vector<Vec2> screenPoints(worldPoints.size());

int successCount = transformer.WorldToScreenBatch(
    worldPoints.data(),
    screenPoints.data(),
    worldPoints.size()
);

std::cout << "Successfully transformed " << successCount << " points" << std::endl;
```

### Visibility Testing

```cpp
// Check if a 3D point would be visible on screen
Vec3 targetPos(150.0f, 250.0f, 350.0f);

if (transformer.IsPointVisible(targetPos)) {
    std::cout << "Target is visible on screen" << std::endl;

    // Get the distance to target
    float distance = transformer.GetDistanceToPoint(targetPos);
    std::cout << "Distance: " << distance << " units" << std::endl;
}
```

### Creating Projection Matrices

```cpp
// Create a perspective projection matrix
float fovY = 90.0f * (M_PI / 180.0f);  // 90 degrees in radians
float aspectRatio = 16.0f / 9.0f;      // 16:9 aspect ratio
float nearPlane = 0.1f;
float farPlane = 1000.0f;

Matrix4x4 projMatrix = W2SUtils::CreatePerspectiveMatrix(
    fovY, aspectRatio, nearPlane, farPlane
);

// Create a look-at view matrix
Vec3 cameraPos(0.0f, 0.0f, 10.0f);
Vec3 targetPos(0.0f, 0.0f, 0.0f);
Vec3 upVector(0.0f, 1.0f, 0.0f);

Matrix4x4 viewMatrix = W2SUtils::CreateLookAtMatrix(
    cameraPos, targetPos, upVector
);

// Combine them for the final transformation
Matrix4x4 combinedMatrix = projMatrix * viewMatrix;
transformer.SetViewMatrix(combinedMatrix);
```

### Quick Transformation Without Class Overhead

```cpp
// For performance-critical scenarios
Vec3 worldPos(100.0f, 200.0f, 300.0f);
Vec2 screenPos;

bool success = W2SUtils::QuickWorldToScreen(
    worldPos, viewMatrix, viewport, screenPos
);

if (success) {
    // Use screen coordinates
}
```

### Custom Viewport Setup

```cpp
// Create viewport with offset (for multi-monitor setups)
Viewport customViewport(1920, 1080, 100.0f, 50.0f);  // Width, Height, X offset, Y offset

WorldToScreenTransform transformer(customViewport);

// Check if a screen point is within the viewport
Vec2 testPoint(500.0f, 600.0f);
if (customViewport.IsPointInside(testPoint)) {
    std::cout << "Point is inside viewport" << std::endl;
}

// Get viewport center
Vec2 center = customViewport.GetCenter();
std::cout << "Viewport center: (" << center.x << ", " << center.y << ")" << std::endl;
```

### Dynamic Viewport Updates

```cpp
WorldToScreenTransform transformer(Viewport(1920, 1080));

// Update viewport when window is resized
void OnWindowResize(int newWidth, int newHeight) {
    Viewport newViewport(newWidth, newHeight);
    transformer.SetViewport(newViewport);
}

// Update view matrix when camera moves
void OnCameraMove(const Matrix4x4& newViewMatrix) {
    transformer.SetViewMatrix(newViewMatrix);
}
```

## Applications

### Game Development

```cpp
// Render health bars above players
for (const auto& player : players) {
    Vec2 screenPos;
    if (transformer.WorldToScreen(player.position, screenPos)) {
        RenderHealthBar(screenPos.x, screenPos.y - 50, player.health);
    }
}
```

### 3D Visualization

```cpp
// Project 3D model vertices to screen for 2D overlay
for (const auto& vertex : model.vertices) {
    Vec2 screenVertex;
    if (transformer.WorldToScreen(vertex, screenVertex)) {
        DrawPoint(screenVertex.x, screenVertex.y);
    }
}
```

### Augmented Reality

```cpp
// Overlay information on real-world objects
Vec3 objectWorldPos = TrackRealWorldObject();
Vec2 screenPos;

if (transformer.WorldToScreen(objectWorldPos, screenPos)) {
    DisplayObjectInfo(screenPos.x, screenPos.y, objectData);
}
```

## Performance Considerations

- Use batch transformations for multiple points
- Cache matrices when possible
- Use `QuickWorldToScreen` for maximum performance
- Validate matrix before intensive operations
- Consider using SIMD operations for large datasets

## Dependencies

- Vector Math Library (included in toolkit)
- C++11 or later
- Standard math library (cmath)
