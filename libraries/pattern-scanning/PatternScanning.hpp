/**
 * @file PatternScanning.hpp
 * @brief High-performance pattern scanning utilities for memory analysis
 * @author Lukas Ernst
 * 
 * Advanced pattern scanning library providing Boyer-Moore algorithm implementation,
 * SIMD optimizations, fuzzy matching, and multi-pattern scanning capabilities.
 * Designed for educational purposes, memory analysis, and signature detection.
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

namespace PatternScanning {

/**
 * @brief Represents a pattern with mask for flexible byte matching
 */
struct Pattern {
    std::vector<uint8_t> bytes;
    std::vector<bool> mask;  // true = must match, false = wildcard
    
    Pattern() = default;
    
    /**
     * @brief Create pattern from string representation
     * @param pattern String like "48 8B 05 ?? ?? ?? ??" where ?? are wildcards
     */
    explicit Pattern(const std::string& pattern);
    
    /**
     * @brief Create pattern from bytes and mask
     */
    Pattern(const std::vector<uint8_t>& bytes, const std::vector<bool>& mask);
    
    /**
     * @brief Get the size of the pattern
     */
    size_t Size() const { return bytes.size(); }
    
    /**
     * @brief Check if pattern is valid
     */
    bool IsValid() const { return !bytes.empty() && bytes.size() == mask.size(); }
};

/**
 * @brief Result of a pattern scan operation
 */
struct ScanResult {
    uintptr_t address;
    size_t offset;
    bool found;
    
    ScanResult() : address(0), offset(0), found(false) {}
    ScanResult(uintptr_t addr, size_t off) : address(addr), offset(off), found(true) {}
    
    operator bool() const { return found; }
};

/**
 * @brief Multiple scan results container
 */
using ScanResults = std::vector<ScanResult>;

/**
 * @brief Boyer-Moore pattern scanner for high-performance searching
 */
class BoyerMooreScanner {
private:
    Pattern pattern_;
    std::vector<int> badCharTable_;
    std::vector<int> goodSuffixTable_;
    
    void BuildBadCharTable();
    void BuildGoodSuffixTable();
    
public:
    explicit BoyerMooreScanner(const Pattern& pattern);
    
    /**
     * @brief Scan for pattern in memory region
     */
    ScanResult Scan(const uint8_t* data, size_t size, uintptr_t baseAddress = 0) const;
    
    /**
     * @brief Find all occurrences of pattern
     */
    ScanResults ScanAll(const uint8_t* data, size_t size, uintptr_t baseAddress = 0) const;
};

/**
 * @brief Simple pattern scanner using standard algorithms
 */
class SimpleScanner {
public:
    /**
     * @brief Scan for pattern in memory region
     */
    static ScanResult Scan(const Pattern& pattern, const uint8_t* data, size_t size, uintptr_t baseAddress = 0);
    
    /**
     * @brief Find all occurrences of pattern
     */
    static ScanResults ScanAll(const Pattern& pattern, const uint8_t* data, size_t size, uintptr_t baseAddress = 0);
    
private:
    static bool MatchesPattern(const Pattern& pattern, const uint8_t* data, size_t offset);
};

/**
 * @brief SIMD-optimized pattern scanner (when available)
 */
class SIMDScanner {
public:
    /**
     * @brief Check if SIMD instructions are available
     */
    static bool IsAvailable();
    
    /**
     * @brief Scan for pattern using SIMD instructions
     * @note Only works for patterns without wildcards or simple patterns
     */
    static ScanResult FastScan(const std::vector<uint8_t>& bytes, const uint8_t* data, size_t size, uintptr_t baseAddress = 0);
};

#ifdef _WIN32
/**
 * @brief Memory region information for Windows
 */
struct MemoryRegion {
    uintptr_t baseAddress;
    size_t size;
    DWORD protection;
    DWORD type;
    std::string moduleName;
    
    bool IsExecutable() const {
        return (protection & PAGE_EXECUTE) || 
               (protection & PAGE_EXECUTE_READ) || 
               (protection & PAGE_EXECUTE_READWRITE);
    }
    
    bool IsReadable() const {
        return (protection & PAGE_READONLY) || 
               (protection & PAGE_READWRITE) || 
               (protection & PAGE_EXECUTE_READ) || 
               (protection & PAGE_EXECUTE_READWRITE);
    }
};

/**
 * @brief Process memory scanner for Windows
 */
class ProcessScanner {
private:
    HANDLE processHandle_;
    DWORD processId_;
    std::vector<MemoryRegion> regions_;
    
    void EnumerateRegions();
    std::vector<uint8_t> ReadMemoryRegion(const MemoryRegion& region);
    
public:
    explicit ProcessScanner(DWORD processId);
    explicit ProcessScanner(HANDLE processHandle);
    ~ProcessScanner();
    
    /**
     * @brief Scan pattern in all readable memory regions
     */
    ScanResults ScanProcess(const Pattern& pattern, bool executableOnly = false);
    
    /**
     * @brief Scan pattern in specific module
     */
    ScanResults ScanModule(const Pattern& pattern, const std::string& moduleName);
    
    /**
     * @brief Scan pattern in specific address range
     */
    ScanResults ScanRange(const Pattern& pattern, uintptr_t startAddress, size_t size);
    
    /**
     * @brief Get all memory regions
     */
    const std::vector<MemoryRegion>& GetRegions() const { return regions_; }
    
    /**
     * @brief Find module by name
     */
    MemoryRegion FindModule(const std::string& moduleName);
};
#endif

/**
 * @brief Pattern utilities and helpers
 */
namespace PatternUtils {
    /**
     * @brief Convert IDA-style pattern to Pattern object
     * @param idaPattern Pattern like "48 8B 05 ? ? ? ?"
     */
    Pattern FromIDAPattern(const std::string& idaPattern);
    
    /**
     * @brief Convert x64dbg-style pattern to Pattern object  
     * @param x64dbgPattern Pattern like "48 8B 05 ?? ?? ?? ??"
     */
    Pattern FromX64DbgPattern(const std::string& x64dbgPattern);
    
    /**
     * @brief Convert Code Cave pattern (common assembly patterns)
     */
    Pattern CreateCodeCavePattern(size_t nopCount = 32);
    
    /**
     * @brief Create pattern for function prologue
     */
    Pattern CreateFunctionPrologue();
    
    /**
     * @brief Create pattern for function epilogue
     */
    Pattern CreateFunctionEpilogue();
    
    /**
     * @brief Convert pattern to string representation
     */
    std::string ToString(const Pattern& pattern);
    
    /**
     * @brief Validate pattern string format
     */
    bool IsValidPatternString(const std::string& pattern);
}

/**
 * @brief Advanced pattern scanning operations
 */
namespace Advanced {
    /**
     * @brief Scan for multiple patterns simultaneously
     */
    struct MultiPattern {
        std::vector<Pattern> patterns;
        std::vector<std::string> names;
        
        void AddPattern(const Pattern& pattern, const std::string& name = "");
    };
    
    /**
     * @brief Multi-pattern scan results
     */
    struct MultiScanResult {
        std::vector<ScanResults> results;
        std::vector<std::string> patternNames;
    };
    
    /**
     * @brief Scan for multiple patterns in parallel
     */
    MultiScanResult ScanMultiple(const MultiPattern& patterns, const uint8_t* data, size_t size, uintptr_t baseAddress = 0);
    
    /**
     * @brief Fuzzy pattern matching with similarity threshold
     */
    struct FuzzyResult {
        uintptr_t address;
        size_t offset;
        float similarity;  // 0.0 to 1.0
        bool found;
        
        FuzzyResult() : address(0), offset(0), similarity(0.0f), found(false) {}
        FuzzyResult(uintptr_t addr, size_t off, float sim) : address(addr), offset(off), similarity(sim), found(true) {}
    };
    
    /**
     * @brief Fuzzy pattern matching
     */
    FuzzyResult FuzzyScan(const Pattern& pattern, const uint8_t* data, size_t size, float threshold = 0.8f, uintptr_t baseAddress = 0);
    
    /**
     * @brief Statistical analysis of memory regions
     */
    struct MemoryStats {
        size_t totalSize;
        size_t executableSize;
        size_t writableSize;
        std::vector<uint8_t> mostCommonBytes;
        std::vector<Pattern> commonPatterns;
        double entropy;  // Shannon entropy
    };
    
    /**
     * @brief Analyze memory region statistics
     */
    MemoryStats AnalyzeMemory(const uint8_t* data, size_t size);
}



} // namespace PatternScanning
