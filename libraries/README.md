# Systems Programming Toolkit - Libraries Collection

This directory contains a comprehensive collection of C++ libraries for systems programming, designed with best practices and following proper C++ library structure.

## Library Overview

### 🔐 [crypto-utils](crypto-utils/)
Advanced cryptographic utilities and hash functions.
- **Features**: MD5 hashing, XOR encryption, string obfuscation, data integrity verification
- **Use Cases**: Security research, data integrity, educational cryptography
- **Files**: `CryptoUtils.hpp`, `CryptoUtils.cpp`, `README.md`

### 🧠 [memory-management](memory-management/)
Professional Windows process memory management interface.
- **Features**: Process attachment, memory reading/writing, module enumeration, protection handling
- **Use Cases**: System administration, debugging, reverse engineering
- **Files**: `MemoryManager.hpp`, `MemoryManager.cpp`, `README.md`

### 🔍 [pattern-scanning](pattern-scanning/)
High-performance pattern scanning utilities for memory analysis.
- **Features**: Boyer-Moore algorithm, SIMD optimization, fuzzy matching, multi-pattern scanning
- **Use Cases**: Reverse engineering, memory analysis, signature detection
- **Files**: `PatternScanning.hpp`, `PatternScanning.cpp`, `README.md`

### ⚙️ [process-tools](process-tools/)
Advanced process management and inspection utilities.
- **Features**: Process discovery, module management, memory operations, inter-process communication
- **Use Cases**: System monitoring, process management, debugging
- **Files**: `ProcessManager.hpp`, `ProcessManager.cpp`, `README.md`

### 📐 [vector-math](vector-math/)
Comprehensive vector mathematics library for 2D and 3D operations.
- **Features**: Vec2/Vec3 classes, geometric operations, interpolation, matrix operations
- **Use Cases**: Graphics programming, game development, scientific computing
- **Files**: `Vector.hpp`, `Vector.cpp`, `README.md`

### 🌍 [world-to-screen](world-to-screen/)
3D to 2D coordinate transformation library.
- **Features**: World-to-screen projection, view matrices, perspective calculations, boundary validation
- **Use Cases**: Computer graphics, game development, augmented reality, visualization
- **Files**: `WorldToScreen.hpp`, `WorldToScreen.cpp`, `README.md`

## Architecture & Best Practices

### 📋 Design Principles

1. **Separation of Concerns**: Header files contain only declarations, implementations are in .cpp files
2. **Template Specialization**: Templates remain in headers for proper instantiation
3. **RAII Pattern**: Automatic resource management with constructors/destructors
4. **Exception Safety**: Proper error handling and resource cleanup
5. **Platform Compatibility**: Windows-specific code with cross-platform fallbacks

### 🏗️ Library Structure

Each library follows this standardized structure:
```
library-name/
├── LibraryName.hpp     # Header with declarations only
├── LibraryName.cpp     # Implementation file
└── README.md          # Detailed library documentation
```

### 💡 Header-Only vs Implementation Split

- **Headers (.hpp)**:
  - Class declarations
  - Function declarations
  - Template definitions (must stay in header)
  - Inline functions (performance-critical small functions only)
  - Constants and enums

- **Implementation (.cpp)**:
  - Function implementations
  - Class method implementations
  - Static variables
  - Platform-specific code

### 🔧 Compilation Guidelines

#### Include Paths
```cpp
#include "libraries/crypto-utils/CryptoUtils.hpp"
#include "libraries/vector-math/Vector.hpp"
// etc.
```

#### Linking Requirements
- **Windows**: Link against `advapi32.lib`, `user32.lib`, `kernel32.lib`
- **Cross-Platform**: Most libraries provide no-op stubs for non-Windows systems

#### Compiler Requirements
- **C++ Standard**: C++17 or later
- **Compilers**: MSVC, GCC, Clang
- **Optimization**: Recommended `-O2` or higher for production builds

## Usage Examples

### Basic Memory Management
```cpp
#include "libraries/memory-management/MemoryManager.hpp"

MemoryManagement::MemoryManager manager;
if (manager.AttachToProcess("notepad.exe") == MemoryManagement::MemoryResult::Success) {
    int value;
    if (manager.Read<int>(0x12345678, value) == MemoryManagement::MemoryResult::Success) {
        std::cout << "Read value: " << value << std::endl;
    }
}
```

### Cryptographic Operations
```cpp
#include "libraries/crypto-utils/CryptoUtils.hpp"

std::string data = "Hello, World!";
std::string hash = MD5::HashString(data);
std::cout << "MD5: " << hash << std::endl;

// String obfuscation
StringObfuscation::RuntimeXor encrypted(data, 0xAA);
std::string decrypted = encrypted.Decrypt();
```

### Vector Mathematics
```cpp
#include "libraries/vector-math/Vector.hpp"

Vec3 position(1.0f, 2.0f, 3.0f);
Vec3 target(4.0f, 5.0f, 6.0f);

float distance = position.Distance(target);
Vec3 direction = (target - position).Normalized();
Vec2 angles = CalculateAngle(position, target);
```

## Testing & Validation

### Compilation Test
Run the provided compilation test script:
```bash
test_libraries_compilation.bat
```

### Unit Testing
Each library includes example usage in its respective README file. Comprehensive unit tests can be found in the `examples/` directory.

### Performance Benchmarks
Performance-critical functions are optimized and benchmarked:
- Pattern scanning: Boyer-Moore algorithm with SIMD optimizations
- Vector operations: Inlined for maximum performance
- Memory management: Minimal overhead with efficient caching

## Integration Guide

### CMake Integration
```cmake
# Add include directories
target_include_directories(your_target PRIVATE libraries/)

# Add source files
target_sources(your_target PRIVATE
    libraries/crypto-utils/CryptoUtils.cpp
    libraries/memory-management/MemoryManager.cpp
    libraries/pattern-scanning/PatternScanning.cpp
    libraries/process-tools/ProcessManager.cpp
    libraries/vector-math/Vector.cpp
    libraries/world-to-screen/WorldToScreen.cpp
)

# Link Windows libraries if needed
if(WIN32)
    target_link_libraries(your_target PRIVATE advapi32 user32 kernel32)
endif()
```

### Visual Studio Integration
1. Add library directories to include paths
2. Add .cpp files to your project
3. Link against required Windows libraries
4. Set C++17 standard or higher

## Platform Support

| Library | Windows | Linux | macOS | Notes |
|---------|---------|--------|-------|-------|
| crypto-utils | ✅ | ✅ | ✅ | Full cross-platform support |
| memory-management | ✅ | 🔄 | 🔄 | Windows-specific, stubs for others |
| pattern-scanning | ✅ | ✅ | ✅ | Cross-platform with Windows optimizations |
| process-tools | ✅ | 🔄 | 🔄 | Windows-specific APIs |
| vector-math | ✅ | ✅ | ✅ | Full cross-platform support |
| world-to-screen | ✅ | ✅ | ✅ | Full cross-platform support |

Legend: ✅ Full Support | 🔄 Limited/Stub Support

## Contributing

When contributing to these libraries:

1. **Follow the established architecture**: Keep declarations in headers, implementations in .cpp files
2. **Maintain backwards compatibility**: Don't break existing APIs
3. **Add comprehensive documentation**: Update README files and inline comments
4. **Test thoroughly**: Ensure cross-platform compatibility where applicable
5. **Follow C++ best practices**: RAII, const-correctness, exception safety

## License

These libraries are part of the Systems Programming Toolkit and are provided for educational and research purposes. See the main project LICENSE file for details.

---

For detailed information about each library, please refer to the individual README files in each library's directory.