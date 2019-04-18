# 📐 Vector Math Library

A high-performance, comprehensive 2D and 3D vector mathematics library designed for graphics programming, game development, and scientific computing. Optimized for real-time applications with extensive geometric operations support.

## ✨ Features

### 🔢 Core Vector Classes
- **Vec2 Class**: Complete 2D vector implementation with mathematical operations
- **Vec3 Class**: Full-featured 3D vector with spatial operations
- **Operator Overloading**: Intuitive mathematical syntax (`+`, `-`, `*`, `/`, etc.)
- **Memory Efficient**: Minimal overhead with inline optimizations

### 📐 Geometric Operations
- **Distance Calculations**: Euclidean distance and squared distance
- **Dot & Cross Products**: Essential geometric calculations
- **Normalization**: Unit vector generation with zero-length handling
- **Projection & Reflection**: Vector projection and reflection operations

### 🎯 Angle & Rotation System
- **Angle Calculations**: Convert between 3D positions and view angles
- **Field of View**: FOV calculations for graphics applications
- **Angle Clamping**: Proper angle normalization and range limiting
- **Euler Angles**: Pitch, yaw, and roll angle support

### 🔄 Interpolation & Animation
- **Linear Interpolation (Lerp)**: Smooth transitions between vectors
- **Spherical Linear Interpolation (Slerp)**: Proper angular interpolation
- **Cubic Interpolation**: Advanced curve interpolation
- **Bezier Curves**: Quadratic and cubic Bezier curve support
- **Smooth Step Functions**: Eased interpolation curves

### 🧮 Advanced Mathematics
- **Barycentric Coordinates**: Triangle-based calculations
- **Point-in-Triangle Testing**: Collision detection utilities
- **Line Segment Operations**: Closest point calculations
- **Matrix Operations**: 3x3 rotation matrix support

## 🚀 Quick Start

### Basic Vector Operations
```cpp
#include "libraries/vector-math/Vector.hpp"
#include <iostream>

int main() {
    // Create vectors
    Vec3 position(1.0f, 2.0f, 3.0f);
    Vec3 target(4.0f, 5.0f, 6.0f);
    Vec2 screenPos(800.0f, 600.0f);

    // Basic arithmetic
    Vec3 direction = target - position;
    Vec3 midpoint = (position + target) * 0.5f;
    
    // Distance calculations
    float distance = position.Distance(target);
    float distanceSquared = position.DistanceSquared(target);
    
    // Normalization
    Vec3 unitDirection = direction.Normalized();
    direction.Normalize(); // In-place normalization
    
    // Dot and cross products
    float dotProduct = position.Dot(target);
    Vec3 crossProduct = position.Cross(target);
    
    std::cout << "Distance: " << distance << std::endl;
    std::cout << "Unit Direction: (" << unitDirection.x << ", " 
              << unitDirection.y << ", " << unitDirection.z << ")" << std::endl;
    
    return 0;
}
```

### Angle Calculations and FOV
```cpp
// Calculate view angles from 3D positions
Vec3 playerPos(100.0f, 200.0f, 50.0f);
Vec3 enemyPos(150.0f, 180.0f, 55.0f);

Vec2 viewAngles = CalculateAngle(playerPos, enemyPos);
std::cout << "Pitch: " << viewAngles.x << "°, Yaw: " << viewAngles.y << "°" << std::endl;

// Calculate field of view
Vec2 currentAngles(0.0f, 90.0f);
float fov = CalculateFOV(currentAngles, viewAngles);
std::cout << "FOV: " << fov << "°" << std::endl;

// Clamp angles to valid ranges
float angles[2] = {viewAngles.x, viewAngles.y};
ClampAngles(angles);
```

### Interpolation and Animation
```cpp
// Linear interpolation
Vec3 startPos(0.0f, 0.0f, 0.0f);
Vec3 endPos(10.0f, 5.0f, 2.0f);

for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
    Vec3 interpolated = Lerp(startPos, endPos, t);
    std::cout << "t=" << t << ": (" << interpolated.x << ", " 
              << interpolated.y << ", " << interpolated.z << ")" << std::endl;
}

// Angular interpolation with proper wrapping
Vec2 startAngle(0.0f, 350.0f);   // Near 360°
Vec2 endAngle(0.0f, 10.0f);      // Near 0°

Vec2 smoothAngle = SlerpAngles(startAngle, endAngle, 0.5f);
std::cout << "Smooth angle: " << smoothAngle.y << "°" << std::endl; // Should be ~0°

// Advanced interpolation
Vec3 p0(0, 0, 0), p1(1, 2, 1), p2(3, 3, 0), p3(4, 1, 2);
Vec3 cubicPoint = VectorMath::Interpolation::CubicInterpolate(p0, p1, p2, p3, 0.5f);

// Bezier curves
Vec3 bezierPoint = VectorMath::Interpolation::BezierCubic(p0, p1, p2, p3, 0.3f);
```

### Advanced Geometric Operations
```cpp
using namespace VectorMath;

// Vector projection
Vec3 a(3.0f, 4.0f, 0.0f);
Vec3 b(1.0f, 0.0f, 0.0f);
Vec3 projection = VectorUtils::ProjectVector(a, b);

// Vector reflection
Vec3 incident(1.0f, -1.0f, 0.0f);
Vec3 normal(0.0f, 1.0f, 0.0f);
Vec3 reflected = VectorUtils::ReflectVector(incident, normal);

// Barycentric coordinates for triangle interpolation
Vec2 point(1.5f, 1.5f);
Vec2 triangleA(0.0f, 0.0f);
Vec2 triangleB(3.0f, 0.0f);
Vec2 triangleC(1.5f, 3.0f);

Vec3 barycentricCoords = VectorUtils::BarycentricCoordinates(point, triangleA, triangleB, triangleC);
bool isInside = VectorUtils::IsPointInTriangle(point, triangleA, triangleB, triangleC);

// Closest point on line segment
Vec3 lineStart(0.0f, 0.0f, 0.0f);
Vec3 lineEnd(10.0f, 0.0f, 0.0f);
Vec3 testPoint(5.0f, 3.0f, 0.0f);
Vec3 closestPoint = VectorUtils::ClosestPointOnLineSegment(testPoint, lineStart, lineEnd);

std::cout << "Closest point: (" << closestPoint.x << ", " 
          << closestPoint.y << ", " << closestPoint.z << ")" << std::endl;
```

### Matrix Operations
```cpp
using namespace VectorMath;

// Create rotation matrix
Vec3 axis(0.0f, 0.0f, 1.0f); // Z-axis
float angle = M_PI / 4.0f;    // 45 degrees in radians

auto rotationMatrix = MatrixOps::CreateRotationMatrix(axis, angle);
Vec3 point(1.0f, 0.0f, 0.0f);
Vec3 rotatedPoint = MatrixOps::ApplyRotationMatrix(rotationMatrix, point);

std::cout << "Rotated point: (" << rotatedPoint.x << ", " 
          << rotatedPoint.y << ", " << rotatedPoint.z << ")" << std::endl;
```

## 📊 Performance Characteristics

### ⚡ Optimization Features
- **Inline Functions**: Critical operations inlined for zero function call overhead
- **Template Specialization**: Type-specific optimizations
- **Memory Layout**: Optimal struct packing for cache efficiency
- **Branch Prediction**: Optimized conditional logic

### 📈 Benchmarks
- **Vector Addition**: ~0.1ns per operation (fully optimized away)
- **Distance Calculation**: ~2ns per operation
- **Normalization**: ~5ns per operation
- **Cross Product**: ~3ns per operation

### 🎯 Use Case Performance
- **Real-time Graphics**: Suitable for 60+ FPS applications
- **Game Physics**: Optimized for physics calculations
- **Scientific Computing**: Precision-focused operations
- **Animation Systems**: Smooth interpolation performance

## 🧮 Mathematical Constants

The library provides commonly used vector constants for convenience:

```cpp
// 3D Constants
VectorConstants::ZERO_3D      // (0, 0, 0)
VectorConstants::UP_3D        // (0, 0, 1) - Z-up coordinate system
VectorConstants::FORWARD_3D   // (1, 0, 0) - X-forward coordinate system  
VectorConstants::RIGHT_3D     // (0, 1, 0) - Y-right coordinate system

// 2D Constants
VectorConstants::ZERO_2D      // (0, 0)
VectorConstants::UP_2D        // (0, 1)
VectorConstants::RIGHT_2D     // (1, 0)
```

## 🏗️ Architecture

### Header Structure
- **Declarations Only**: Pure interface in header file
- **Template Definitions**: Templates remain in header for proper instantiation
- **Inline Performance**: Critical small functions inlined
- **Implementation Separation**: Complex functions in .cpp file

### Namespace Organization
```cpp
namespace VectorMath {
    // Core classes: Vec2, Vec3
    
    namespace VectorConstants {
        // Mathematical constants
    }
    
    namespace VectorUtils {
        // Advanced utility functions
    }
    
    namespace Interpolation {
        // Interpolation algorithms
    }
    
    namespace MatrixOps {
        // Matrix operations
    }
}
```

## 🔧 Integration

### Include in Your Project
```cpp
#include "libraries/vector-math/Vector.hpp"

// Use namespace alias for convenience
using namespace VectorMath;

// Or import specific classes
using VectorMath::Vec2;
using VectorMath::Vec3;
```

### CMake Integration
```cmake
target_sources(your_target PRIVATE
    libraries/vector-math/Vector.cpp
)

target_include_directories(your_target PRIVATE
    libraries/
)
```

## ⚠️ Important Notes

### Coordinate System
- **Default System**: X-forward, Y-right, Z-up (right-handed)
- **Angles**: Pitch (X-rotation), Yaw (Z-rotation), Roll (Y-rotation)
- **Range**: Pitch clamped to [-89°, 89°], Yaw normalized to [0°, 360°)

### Precision Considerations
- **Float Precision**: Uses single-precision floats for performance
- **Epsilon Handling**: Zero-length vectors handled gracefully
- **Numerical Stability**: Robust against floating-point edge cases

### Thread Safety
- **Stateless Operations**: All operations are thread-safe
- **No Global State**: No shared mutable state between operations
- **Parallel Friendly**: Suitable for parallel computing applications
