/**
 * @file PatternScanning.cpp
 * @brief Implementation of high-performance pattern scanning utilities
 * @author Lukas Ernst
 * 
 * Implementation of advanced pattern scanning algorithms including Boyer-Moore,
 * SIMD optimizations, fuzzy matching, and process memory scanning. Provides
 * cross-platform compatibility and performance optimizations for educational purposes.
 */

#include "PatternScanning.hpp"
#include <cstring>
#include <thread>
#include <algorithm>
#include <sstream>
#include <memory>
#include <vector>
#include <stdexcept>

#ifdef _MSC_VER
#include <intrin.h>
#elif defined(__GNUC__)
#include <x86intrin.h>
#endif

namespace PatternScanning {

// Pattern implementation
Pattern::Pattern(const std::string& pattern) {
    std::istringstream iss(pattern);
    std::string token;
    
    while (iss >> token) {
        if (token == "?" || token == "??") {
            bytes.push_back(0x00);
            mask.push_back(false);  // Wildcard
        } else {
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                bytes.push_back(byte);
                mask.push_back(true);   // Must match
            } catch (...) {
                bytes.clear();
                mask.clear();
                break;  // Invalid pattern
            }
        }
    }
}

Pattern::Pattern(const std::vector<uint8_t>& bytes, const std::vector<bool>& mask) 
    : bytes(bytes), mask(mask) {
    if (bytes.size() != mask.size()) {
        this->bytes.clear();
        this->mask.clear();
    }
}

// SimpleScanner implementation
ScanResult SimpleScanner::Scan(const Pattern& pattern, const uint8_t* data, size_t size, uintptr_t baseAddress) {
    if (!pattern.IsValid() || !data || size < pattern.Size()) {
        return ScanResult();
    }
    
    for (size_t i = 0; i <= size - pattern.Size(); ++i) {
        if (MatchesPattern(pattern, data, i)) {
            return ScanResult(baseAddress + i, i);
        }
    }
    
    return ScanResult();
}

ScanResults SimpleScanner::ScanAll(const Pattern& pattern, const uint8_t* data, size_t size, uintptr_t baseAddress) {
    ScanResults results;
    if (!pattern.IsValid() || !data || size < pattern.Size()) {
        return results;
    }
    
    for (size_t i = 0; i <= size - pattern.Size(); ++i) {
        if (MatchesPattern(pattern, data, i)) {
            results.emplace_back(baseAddress + i, i);
        }
    }
    
    return results;
}

bool SimpleScanner::MatchesPattern(const Pattern& pattern, const uint8_t* data, size_t offset) {
    for (size_t i = 0; i < pattern.Size(); ++i) {
        if (pattern.mask[i] && data[offset + i] != pattern.bytes[i]) {
            return false;
        }
    }
    return true;
}

// BoyerMooreScanner implementation
BoyerMooreScanner::BoyerMooreScanner(const Pattern& pattern) 
    : pattern_(pattern) {
    if (pattern_.IsValid()) {
        BuildBadCharTable();
        BuildGoodSuffixTable();
    }
}

void BoyerMooreScanner::BuildBadCharTable() {
    badCharTable_.resize(256, static_cast<int>(pattern_.Size()));
    
    for (size_t i = 0; i < pattern_.Size() - 1; ++i) {
        if (pattern_.mask[i]) {
            badCharTable_[pattern_.bytes[i]] = static_cast<int>(pattern_.Size() - 1 - i);
        }
    }
}

void BoyerMooreScanner::BuildGoodSuffixTable() {
    size_t patternLength = pattern_.Size();
    goodSuffixTable_.resize(patternLength, static_cast<int>(patternLength));
    
    std::vector<int> borderTable(patternLength + 1, 0);
    
    // Compute border table
    int i = static_cast<int>(patternLength);
    int j = static_cast<int>(patternLength) + 1;
    borderTable[i] = j;
    
    while (i > 0) {
        while (j <= static_cast<int>(patternLength) && 
               (j == static_cast<int>(patternLength) || 
                !pattern_.mask[i - 1] || !pattern_.mask[j - 1] ||
                pattern_.bytes[i - 1] != pattern_.bytes[j - 1])) {
            if (goodSuffixTable_[j - 1] == static_cast<int>(patternLength)) {
                goodSuffixTable_[j - 1] = j - i;
            }
            j = borderTable[j];
        }
        i--;
        j--;
        borderTable[i] = j;
    }
    
    // Complete good suffix table
    j = borderTable[0];
    for (i = 0; i <= static_cast<int>(patternLength); i++) {
        if (goodSuffixTable_[i] == static_cast<int>(patternLength)) {
            goodSuffixTable_[i] = j;
        }
        if (i == j) {
            j = borderTable[j];
        }
    }
}

ScanResult BoyerMooreScanner::Scan(const uint8_t* data, size_t size, uintptr_t baseAddress) const {
    if (!pattern_.IsValid() || !data || size < pattern_.Size()) {
        return ScanResult();
    }
    
    size_t patternLength = pattern_.Size();
    size_t textLength = size;
    
    size_t shift = 0;
    while (shift <= textLength - patternLength) {
        int j = static_cast<int>(patternLength) - 1;
        
        // Check pattern from right to left
        while (j >= 0 && 
               (!pattern_.mask[j] || pattern_.bytes[j] == data[shift + j])) {
            j--;
        }
        
        if (j < 0) {
            // Pattern found
            return ScanResult(baseAddress + shift, shift);
        } else {
            // Calculate shift using bad character and good suffix heuristics
            int badCharShift = badCharTable_[data[shift + j]] - static_cast<int>(patternLength) + 1 + j;
            int goodSuffixShift = goodSuffixTable_[j];
            shift += (badCharShift > goodSuffixShift) ? badCharShift : goodSuffixShift;
        }
    }
    
    return ScanResult();
}

ScanResults BoyerMooreScanner::ScanAll(const uint8_t* data, size_t size, uintptr_t baseAddress) const {
    ScanResults results;
    if (!pattern_.IsValid() || !data || size < pattern_.Size()) {
        return results;
    }
    
    size_t patternLength = pattern_.Size();
    size_t textLength = size;
    
    size_t shift = 0;
    while (shift <= textLength - patternLength) {
        int j = static_cast<int>(patternLength) - 1;
        
        // Check pattern from right to left
        while (j >= 0 && 
               (!pattern_.mask[j] || pattern_.bytes[j] == data[shift + j])) {
            j--;
        }
        
        if (j < 0) {
            // Pattern found
            results.emplace_back(baseAddress + shift, shift);
            shift += (shift + patternLength < textLength) ? 
                     goodSuffixTable_[0] : 1;
        } else {
            // Calculate shift using bad character and good suffix heuristics
            int badCharShift = badCharTable_[data[shift + j]] - static_cast<int>(patternLength) + 1 + j;
            int goodSuffixShift = goodSuffixTable_[j];
            shift += (badCharShift > goodSuffixShift) ? badCharShift : goodSuffixShift;
        }
    }
    
    return results;
}

// SIMDScanner implementation
bool SIMDScanner::IsAvailable() {
    // Check for SSE2 support (basic requirement)
    #ifdef _WIN32
        #ifdef _MSC_VER
            int cpuInfo[4];
            __cpuid(cpuInfo, 1);
            return (cpuInfo[3] & (1 << 26)) != 0; // SSE2 bit
        #elif defined(__GNUC__)
            unsigned int eax, ebx, ecx, edx;
            __get_cpuid(1, &eax, &ebx, &ecx, &edx);
            return (edx & (1 << 26)) != 0; // SSE2 bit
        #else
            return false;
        #endif
    #else
        return false; // Not implemented for non-Windows
    #endif
}

ScanResult SIMDScanner::FastScan(const std::vector<uint8_t>& bytes, const uint8_t* data, size_t size, uintptr_t baseAddress) {
    if (bytes.empty() || !data || size < bytes.size()) {
        return ScanResult();
    }
    
    // Simple implementation - use standard search for now
    // In a real implementation, you would use SIMD instructions like _mm_cmpeq_epi8
    for (size_t i = 0; i <= size - bytes.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < bytes.size(); ++j) {
            if (data[i + j] != bytes[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return ScanResult(baseAddress + i, i);
        }
    }
    
    return ScanResult();
}

#ifdef _WIN32
// ProcessScanner implementation
ProcessScanner::ProcessScanner(DWORD processId) 
    : processHandle_(INVALID_HANDLE_VALUE), processId_(processId) {
    processHandle_ = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle_ != INVALID_HANDLE_VALUE) {
        EnumerateRegions();
    }
}

ProcessScanner::ProcessScanner(HANDLE processHandle) 
    : processHandle_(processHandle), processId_(::GetProcessId(processHandle)) {
    if (processHandle_ != INVALID_HANDLE_VALUE) {
        EnumerateRegions();
    }
}

ProcessScanner::~ProcessScanner() {
    if (processHandle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(processHandle_);
    }
}

void ProcessScanner::EnumerateRegions() {
    regions_.clear();
    
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t address = 0;
    
    while (VirtualQueryEx(processHandle_, reinterpret_cast<LPVOID>(address), &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_GUARD) == 0) {
            MemoryRegion region;
            region.baseAddress = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
            region.size = mbi.RegionSize;
            region.protection = mbi.Protect;
            region.type = mbi.Type;
            
            // Try to get module name
            char moduleName[MAX_PATH] = {0};
            if (GetModuleBaseNameA(processHandle_, reinterpret_cast<HMODULE>(mbi.AllocationBase), moduleName, MAX_PATH)) {
                region.moduleName = moduleName;
            } else {
                region.moduleName = "Unknown";
            }
            
            regions_.push_back(region);
        }
        
        address = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    }
}

std::vector<uint8_t> ProcessScanner::ReadMemoryRegion(const MemoryRegion& region) {
    std::vector<uint8_t> buffer(region.size);
    SIZE_T bytesRead;
    
    if (ReadProcessMemory(processHandle_, reinterpret_cast<LPCVOID>(region.baseAddress),
                         buffer.data(), region.size, &bytesRead)) {
        buffer.resize(bytesRead);
        return buffer;
    }
    
    return std::vector<uint8_t>();
}

ScanResults ProcessScanner::ScanProcess(const Pattern& pattern, bool executableOnly) {
    ScanResults allResults;
    
    BoyerMooreScanner scanner(pattern);
    
    for (const auto& region : regions_) {
        if (!region.IsReadable()) continue;
        if (executableOnly && !region.IsExecutable()) continue;
        
        auto buffer = ReadMemoryRegion(region);
        if (!buffer.empty()) {
            auto results = scanner.ScanAll(buffer.data(), buffer.size(), region.baseAddress);
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
    }
    
    return allResults;
}

ScanResults ProcessScanner::ScanModule(const Pattern& pattern, const std::string& moduleName) {
    for (const auto& region : regions_) {
        if (region.moduleName == moduleName && region.IsReadable()) {
            auto buffer = ReadMemoryRegion(region);
            if (!buffer.empty()) {
                BoyerMooreScanner scanner(pattern);
                return scanner.ScanAll(buffer.data(), buffer.size(), region.baseAddress);
            }
        }
    }
    
    return ScanResults();
}

ScanResults ProcessScanner::ScanRange(const Pattern& pattern, uintptr_t startAddress, size_t size) {
    std::vector<uint8_t> buffer(size);
    SIZE_T bytesRead;
    
    if (ReadProcessMemory(processHandle_, reinterpret_cast<LPCVOID>(startAddress),
                         buffer.data(), size, &bytesRead)) {
        buffer.resize(bytesRead);
        BoyerMooreScanner scanner(pattern);
        return scanner.ScanAll(buffer.data(), buffer.size(), startAddress);
    }
    
    return ScanResults();
}

MemoryRegion ProcessScanner::FindModule(const std::string& moduleName) {
    for (const auto& region : regions_) {
        if (region.moduleName == moduleName) {
            return region;
        }
    }
    return MemoryRegion{};
}
#endif

// Advanced namespace implementation
namespace Advanced {

void MultiPattern::AddPattern(const Pattern& pattern, const std::string& name) {
    patterns.push_back(pattern);
    names.push_back(name.empty() ? ("Pattern_" + std::to_string(patterns.size())) : name);
}

MultiScanResult ScanMultiple(const MultiPattern& patterns, const uint8_t* data, size_t size, uintptr_t baseAddress) {
    MultiScanResult result;
    result.results.resize(patterns.patterns.size());
    result.patternNames = patterns.names;
    
    // Use threading for parallel scanning when beneficial
    if (patterns.patterns.size() > 1 && size > 1024 * 1024) {
        std::vector<std::thread> threads;
        
        for (size_t i = 0; i < patterns.patterns.size(); ++i) {
            threads.emplace_back([&, i]() {
                BoyerMooreScanner scanner(patterns.patterns[i]);
                result.results[i] = scanner.ScanAll(data, size, baseAddress);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    } else {
        // Sequential scanning for small patterns or data
        for (size_t i = 0; i < patterns.patterns.size(); ++i) {
            BoyerMooreScanner scanner(patterns.patterns[i]);
            result.results[i] = scanner.ScanAll(data, size, baseAddress);
        }
    }
    
    return result;
}

FuzzyResult FuzzyScan(const Pattern& pattern, const uint8_t* data, size_t size, float threshold, uintptr_t baseAddress) {
    if (!pattern.IsValid() || !data || size < pattern.Size() || threshold < 0.0f || threshold > 1.0f) {
        return FuzzyResult();
    }
    
    float bestSimilarity = 0.0f;
    size_t bestOffset = 0;
    
    for (size_t i = 0; i <= size - pattern.Size(); ++i) {
        int matches = 0;
        int totalChecks = 0;
        
        for (size_t j = 0; j < pattern.Size(); ++j) {
            if (pattern.mask[j]) {
                totalChecks++;
                if (data[i + j] == pattern.bytes[j]) {
                    matches++;
                }
            }
        }
        
        if (totalChecks > 0) {
            float similarity = static_cast<float>(matches) / totalChecks;
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestOffset = i;
            }
            
            if (similarity >= threshold) {
                return FuzzyResult(baseAddress + i, i, similarity);
            }
        }
    }
    
    // Return best match even if below threshold
    if (bestSimilarity > 0.0f) {
        return FuzzyResult(baseAddress + bestOffset, bestOffset, bestSimilarity);
    }
    
    return FuzzyResult();
}

MemoryStats AnalyzeMemory(const uint8_t* data, size_t size) {
    MemoryStats stats = {};
    stats.totalSize = size;
    
    if (!data || size == 0) {
        return stats;
    }
    
    // Count byte frequencies
    std::vector<int> byteFreq(256, 0);
    for (size_t i = 0; i < size; ++i) {
        byteFreq[data[i]]++;
    }
    
    // Calculate Shannon entropy
    double entropy = 0.0;
    for (int freq : byteFreq) {
        if (freq > 0) {
            double p = static_cast<double>(freq) / size;
            entropy -= p * std::log2(p);
        }
    }
    stats.entropy = entropy;
    
    // Find most common bytes
    std::vector<std::pair<uint8_t, int>> sortedBytes;
    for (int i = 0; i < 256; ++i) {
        if (byteFreq[i] > 0) {
            sortedBytes.emplace_back(static_cast<uint8_t>(i), byteFreq[i]);
        }
    }
    
    std::sort(sortedBytes.begin(), sortedBytes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Store top 10 most common bytes
    size_t maxElements = (sortedBytes.size() < 10) ? sortedBytes.size() : 10;
    for (size_t i = 0; i < maxElements; ++i) {
        stats.mostCommonBytes.push_back(sortedBytes[i].first);
    }
    
    return stats;
}

} // namespace Advanced

// PatternUtils implementation
namespace PatternUtils {

Pattern FromIDAPattern(const std::string& idaPattern) {
    std::string normalized = idaPattern;
    std::replace(normalized.begin(), normalized.end(), '?', ' ');
    // Replace single ? with ??
    size_t pos = 0;
    while ((pos = normalized.find("? ", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "?? ");
        pos += 3;
    }
    return Pattern(normalized);
}

Pattern FromX64DbgPattern(const std::string& x64dbgPattern) {
    return Pattern(x64dbgPattern);  // Direct conversion
}

Pattern CreateCodeCavePattern(size_t nopCount) {
    std::vector<uint8_t> bytes(nopCount, 0x90);  // NOP instructions
    std::vector<bool> mask(nopCount, true);
    return Pattern(bytes, mask);
}

Pattern CreateFunctionPrologue() {
    // Common x64 function prologue: push rbp; mov rbp, rsp
    return Pattern("55 48 89 E5");
}

Pattern CreateFunctionEpilogue() {
    // Common x64 function epilogue: pop rbp; ret
    return Pattern("5D C3");
}

std::string ToString(const Pattern& pattern) {
    std::string result;
    for (size_t i = 0; i < pattern.Size(); ++i) {
        if (i > 0) result += " ";
        
        if (pattern.mask[i]) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02X", pattern.bytes[i]);
            result += hex;
        } else {
            result += "??";
        }
    }
    return result;
}

bool IsValidPatternString(const std::string& pattern) {
    if (pattern.empty()) {
        return false;
    }
    
    std::istringstream iss(pattern);
    std::string token;
    
    while (iss >> token) {
        if (token == "?" || token == "??") {
            continue;  // Valid wildcards
        } else {
            // Check if it's a valid hex byte
            if (token.length() == 0 || token.length() > 3) {
                return false;  // Invalid length
            }
            
            for (char c : token) {
                if (!std::isdigit(c) && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f')) {
                    return false;  // Invalid hex character
                }
            }
            
            try {
                unsigned long value = std::stoul(token, nullptr, 16);
                if (value > 0xFF) {
                    return false;  // Value too large for a byte
                }
            } catch (...) {
                return false;  // Invalid hex string
            }
        }
    }
    
    return true;  // All tokens are valid
}

} // namespace PatternUtils

} // namespace PatternScanning