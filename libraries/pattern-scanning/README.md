# 🔍 Pattern Scanning Library

A high-performance, comprehensive pattern scanning library designed for memory analysis, reverse engineering, and signature detection. Features multiple scanning algorithms including Boyer-Moore, SIMD optimizations, and fuzzy matching capabilities.

## ✨ Features

### 🚀 High-Performance Algorithms
- **Boyer-Moore Scanner**: Advanced string matching with optimal complexity
- **Simple Scanner**: Lightweight implementation for basic pattern matching
- **SIMD Scanner**: Vectorized operations for maximum performance (when available)
- **Batch Processing**: Scan multiple patterns simultaneously

### 🎯 Pattern Types & Formats
- **Hex Pattern Support**: Standard "48 8B 05 ?? ?? ?? ??" format
- **IDA Pattern Compatibility**: Import patterns from IDA Pro
- **x64dbg Pattern Support**: Compatible with x64dbg pattern format
- **Wildcard Support**: Flexible pattern matching with wildcards
- **Binary Pattern Matching**: Direct byte array pattern scanning

### 🔬 Advanced Scanning Features
- **Multi-Pattern Scanning**: Scan for multiple patterns in parallel
- **Fuzzy Pattern Matching**: Find approximate matches with similarity threshold
- **Memory Region Analysis**: Statistical analysis and entropy calculations
- **Process Memory Scanning**: Direct Windows process memory scanning
- **Module-Specific Scanning**: Target specific loaded modules

### 📊 Analysis & Statistics
- **Memory Statistics**: Entropy, byte frequency, common patterns
- **Performance Metrics**: Scanning speed and efficiency measurements
- **Pattern Validation**: Verify pattern format and structure
- **Result Aggregation**: Comprehensive scan result management

## 🚀 Quick Start

### Basic Pattern Scanning
```cpp
#include "libraries/pattern-scanning/PatternScanning.hpp"
#include <iostream>

int main() {
    // Create a pattern from hex string
    PatternScanning::Pattern pattern("48 8B 05 ?? ?? ?? ??");
    
    if (!pattern.IsValid()) {
        std::cerr << "Invalid pattern format!" << std::endl;
        return 1;
    }

    // Sample memory data to search
    uint8_t memory[] = {
        0x48, 0x8B, 0x05, 0x12, 0x34, 0x56, 0x78,  // Match!
        0x90, 0x90, 0x90,                           // NOP padding
        0x48, 0x8B, 0x05, 0xAB, 0xCD, 0xEF, 0x01   // Another match!
    };

    // Scan using simple scanner
    auto result = PatternScanning::SimpleScanner::Scan(
        pattern, memory, sizeof(memory), 0x1000 // Base address 0x1000
    );

    if (result) {
        std::cout << "Pattern found at address: 0x" << std::hex << result.address << std::endl;
        std::cout << "Offset in buffer: " << std::dec << result.offset << std::endl;
    } else {
        std::cout << "Pattern not found" << std::endl;
    }

    return 0;
}
```

## Pattern Formats

### Standard Format
```cpp
// Bytes with wildcards
Pattern pattern("48 8B 05 ?? ?? ?? ??");
```

### IDA Pro Style
```cpp
// Single ? wildcards  
Pattern pattern = PatternUtils::FromIDAPattern("48 8B 05 ? ? ? ?");
```

### Binary Data

```cpp
// Direct byte vector
std::vector<uint8_t> bytes = {0x48, 0x8B, 0x05};
std::vector<bool> mask = {true, true, true};
Pattern pattern(bytes, mask);
```

## Scanning Algorithms

### Simple Scanner

Best for small patterns and learning:

```cpp
auto result = SimpleScanner::Scan(pattern, data, size);
auto allResults = SimpleScanner::ScanAll(pattern, data, size);
```

### Boyer-Moore Scanner

Optimized for large memory regions:

```cpp
BoyerMooreScanner scanner(pattern);
auto result = scanner.Scan(data, size);
auto allResults = scanner.ScanAll(data, size);
```

### SIMD Scanner

Hardware-accelerated for specific patterns:

```cpp
if (SIMDScanner::IsAvailable()) {
    std::vector<uint8_t> exactBytes = {0x48, 0x8B, 0x05, 0x12};
    auto result = SIMDScanner::FastScan(exactBytes, data, size);
}
```

## Process Memory Scanning (Windows)

```cpp
#ifdef _WIN32
// Scan target process
DWORD processId = 1234;
ProcessScanner scanner(processId);

// Scan all memory regions
auto results = scanner.ScanProcess(pattern);

// Scan specific module
auto moduleResults = scanner.ScanModule(pattern, "kernel32.dll");

// Scan address range
auto rangeResults = scanner.ScanRange(pattern, 0x140000000, 0x1000);
#endif
```

## Advanced Features

### Multi-Pattern Scanning

```cpp
Advanced::MultiPattern multiPattern;
multiPattern.AddPattern(Pattern("48 8B 05 ?? ?? ?? ??"), "GetModuleHandle");
multiPattern.AddPattern(Pattern("FF 25 ?? ?? ?? ??"), "JMP_Instruction");

auto results = Advanced::ScanMultiple(multiPattern, data, size);
```

### Fuzzy Pattern Matching

```cpp
auto fuzzyResult = Advanced::FuzzyScan(pattern, data, size, 0.85f);
if (fuzzyResult.found) {
    std::cout << "Fuzzy match with " << (fuzzyResult.similarity * 100)
              << "% similarity" << std::endl;
}
```

### Memory Analysis

```cpp
auto stats = Advanced::AnalyzeMemory(data, size);
std::cout << "Memory entropy: " << stats.entropy << std::endl;
std::cout << "Executable size: " << stats.executableSize << " bytes" << std::endl;
```

## Common Patterns

### Function Signatures

```cpp
// Standard function prologue (x64)
auto prologue = PatternUtils::CreateFunctionPrologue();  // "55 48 89 E5"

// Function epilogue
auto epilogue = PatternUtils::CreateFunctionEpilogue();  // "5D C3"

// Code cave (NOP sled)
auto codeCave = PatternUtils::CreateCodeCavePattern(32); // 32 NOPs
```

### Assembly Instructions

```cpp
// CALL instruction
Pattern callPattern("E8 ?? ?? ?? ??");

// JMP instruction
Pattern jmpPattern("E9 ?? ?? ?? ??");

// MOV instruction with displacement
Pattern movPattern("48 8B ?? ?? ?? ?? ??");
```

## Performance Tips

1. **Choose the Right Scanner**:

   - `SimpleScanner`: Small patterns, educational use
   - `BoyerMooreScanner`: Large memory regions, complex patterns
   - `SIMDScanner`: Simple exact-match patterns, performance critical

2. **Pattern Design**:

   - Minimize wildcards for better performance
   - Use longer patterns to reduce false positives
   - Place distinctive bytes at the beginning

3. **Memory Access**:
   - Scan readable memory regions only
   - Use batch operations for multiple patterns
   - Consider memory layout and caching

## Memory Region Analysis

```cpp
#ifdef _WIN32
ProcessScanner scanner(processId);
auto regions = scanner.GetRegions();

for (const auto& region : regions) {
    std::cout << "Region: 0x" << std::hex << region.baseAddress
              << " Size: " << std::dec << region.size
              << " Module: " << region.moduleName << std::endl;

    if (region.IsExecutable()) {
        // Scan executable regions for code patterns
        auto codeResults = scanner.ScanRange(codePattern,
                                           region.baseAddress,
                                           region.size);
    }
}
#endif
```

## Error Handling

```cpp
// Check pattern validity
if (!pattern.IsValid()) {
    std::cerr << "Invalid pattern format" << std::endl;
    return;
}

// Check scan results
auto result = scanner.Scan(pattern, data, size);
if (!result) {
    std::cout << "Pattern not found" << std::endl;
} else {
    std::cout << "Found at offset: " << result.offset << std::endl;
}
```

## Integration

This is a header-only library. Simply include the header:

```cpp
#include "PatternScanning.hpp"
```

### Dependencies

- C++11 or later
- Windows API (for process scanning on Windows)
- Optional: SIMD support (SSE2/AVX)

## Use Cases

- **Reverse Engineering**: Find function signatures and code patterns
- **Game Modding**: Locate game functions and data structures
- **Security Research**: Analyze malware and binary patterns
- **Memory Forensics**: Search for specific byte patterns in memory dumps
- **Binary Analysis**: Identify common code constructs and compiler patterns

## Thread Safety

- Pattern objects are immutable after construction (thread-safe)
- Scanner classes are not thread-safe for concurrent modifications
- Multiple threads can safely use different scanner instances
- Process scanning requires proper synchronization for shared process handles

## Limitations

- SIMD scanner only works with exact-match patterns (no wildcards)
- Process scanning is currently Windows-specific
- Large pattern sizes may impact performance
- Memory access permissions must be considered for process scanning
