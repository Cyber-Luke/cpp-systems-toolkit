# Systems Programming Toolkit

A collection of C++ libraries for systems programming, memory management, cryptography, pattern scanning, and 3D mathematics. Built for educational purposes to demonstrate low-level programming concepts and optimization techniques.

> ⚠️ **Important Notice**: This is an **educational project** and is **NOT intended for production use**. The project lacks enterprise-grade security features and error handling required for production environments.

## Features & Performance

### Cryptography
- **MD5 Hashing**: RFC 1321 compliant, 376 MB/s throughput
- **XOR Encryption**: 2.6 GB/s peak performance, multi-key support
- **CRC32 Checksums**: Data integrity verification (757 MB/s)
- **HMAC Authentication**: Keyed-hash message authentication
- **Secure Random**: Cryptographically secure byte generation
- **String Obfuscation**: Compile-time and runtime protection

### Memory Management
- **Memory Operations**: 1.6M reads/sec, 1.3M writes/sec
- **Process Memory Access**: Read/write external process memory
- **Module Enumeration**: List loaded modules and their base addresses
- **Memory Protection**: Change memory page permissions
- **Zero Memory Leaks**: Comprehensive leak detection and prevention

### Pattern Scanning
- **Boyer-Moore Algorithm**: 2.7-4.5x faster than simple scanning
- **Peak Throughput**: 1.77 GB/s on large datasets
- **SIMD Optimization**: Hardware-accelerated scanning
- **Multi-pattern Search**: Scan for multiple patterns simultaneously
- **Process Memory Scanning**: Search patterns in running processes

### Vector Mathematics
- **Sub-nanosecond Performance**: Most operations <1ns
- **Dot Product**: 1.325 billion operations/sec sustained
- **3D Vector Operations**: Add, subtract, multiply, normalize
- **Matrix Transformations**: 4x4 matrix operations for 3D graphics
- **Zero Heap Allocation**: Stack-optimized for maximum performance

### World-to-Screen Projection
- **Transform Performance**: 9.9 billion world-to-screen ops/sec
- **Matrix Operations**: 52M 4x4 matrix multiplications/sec
- **3D to 2D Conversion**: Transform 3D coordinates to screen space
- **Real-time Graphics**: 60+ FPS with thousands of objects
- **3D Pipeline**: Complete perspective and orthographic projection

### Process Management
- **Memory Operations**: 5.3M reads/sec, 1.4M writes/sec
- **Process Enumeration**: List running processes by name or ID
- **Window Management**: Find and interact with process windows
- **Module Enumeration**: System scanning in 80.65μs average
- **Security Validation**: Proper privilege checks and validation

*All performance data benchmarked on AMD Ryzen 5 2600 @ 4.0GHz with MSVC /O2 optimization*

## Quick Start

1. **Build and run demos**
   ```cmd
   .\scripts\all_demo.bat
   ```

2. **Individual library demos**
   ```cmd
   .\scripts\memory_demo.bat
   .\scripts\crypto_demo.bat
   .\scripts\pattern_scanning_demo.bat
   .\scripts\vector_math_demo.bat
   .\scripts\world_to_screen_demo.bat
   .\scripts\process_manager_demo.bat
   ```

## Educational Use & Limitations

### **NOT FOR PRODUCTION USE**

This project is **strictly for educational and demonstration purposes**. It should **NEVER be deployed in production environments** due to the following limitations:

#### Security Limitations
- **Basic Error Handling**: Limited validation and error recovery
- **No Thread Safety**: Global variables without synchronization
- **Platform Specific**: Windows-only implementation
- **Memory Safety**: Manual resource management without comprehensive guards
- **Privilege Requirements**: Some functions require elevated permissions

#### Scalability & Reliability Issues
- **No Logging System**: Basic output without comprehensive monitoring
- **Single-threaded**: Most operations are not thread-safe
- **No Configuration**: Hardcoded values and paths
- **Limited Testing**: Educational test coverage, not production validation

#### Maintenance Status
- **Educational Project**: Designed for learning, not enterprise use
- **No Active Maintenance**: Not intended for ongoing development
- **No Support**: Community project without official support

### Suitable Use Cases

This project **IS suitable** for:

- **Learning Systems Programming**: Understanding low-level Windows APIs
- **Algorithm Study**: Implementing Boyer-Moore, matrix math, cryptography
- **Performance Analysis**: Studying optimization techniques and benchmarking
- **Educational Projects**: Teaching C++ and systems programming concepts
- **Code Reference**: Understanding implementation patterns and techniques

## License

MIT License - see [LICENSE](LICENSE) file for details.