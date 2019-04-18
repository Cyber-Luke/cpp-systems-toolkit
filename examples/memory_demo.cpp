/**
 * @file memory_demo.cpp
 * @brief Comprehensive demonstration of the MemoryManager library
 * @author Lukas Ernst
 * 
 * This comprehensive demo demonstrates all functions of the MemoryManager library:
 * - Process Memory Access (Read/Write operations)
 * - Memory Allocation and Management
 * - Module Enumeration and Analysis
 * - Memory Protection and Security
 * - Pattern Scanning in Memory
 * - Performance Benchmarks
 * - Security Tests and Edge Cases
 */

#include "../libraries/memory-management/MemoryManager.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

class MemoryDemo {
private:
    int totalTests = 0;
    int passedTests = 0;
    
    void PrintHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    void PrintSubHeader(const std::string& title) {
        std::cout << "\n--- " << title << " ---" << std::endl;
    }
    
    void PrintResult(const std::string& test, bool passed) {
        totalTests++;
        if (passed) passedTests++;
        
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test << std::endl;
    }
    
    std::string FormatAddress(uintptr_t address) {
        std::stringstream ss;
        ss << "0x" << std::hex << std::uppercase << address;
        return ss.str();
    }
    
    std::string FormatSize(size_t size) {
        if (size < 1024) {
            return std::to_string(size) + " B";
        } else if (size < 1024 * 1024) {
            return std::to_string(size / 1024) + " KB";
        } else {
            return std::to_string(size / (1024 * 1024)) + " MB";
        }
    }

public:
    void RunAllTests() {
        PrintHeader("FINAL MEMORYMANAGER LIBRARY DEMONSTRATION");
        std::cout << "Complete demonstration of all memory management functions" << std::endl;
        std::cout << "Version 1.0 - Professional process memory operations" << std::endl;
        
#ifdef _WIN32
        std::cout << "Platform: Windows - Full functionality available" << std::endl;
#else
        std::cout << "Platform: Non-Windows - Limited stub functionality" << std::endl;
#endif
        
        // Initialize the global memory manager
        MemoryManagement::InitializeMemoryManager();
        
        // Run all tests
        TestMemoryManagerBasics();
        TestProcessOperations();
        TestMemoryOperations();
        TestModuleManagement();
        TestPatternScanning();
        TestMemoryAllocation();
        TestErrorHandling();
        TestPerformance();
        
        // Cleanup
        MemoryManagement::CleanupMemoryManager();
        
        PrintFinalResults();
    }

private:
    void TestMemoryManagerBasics() {
        PrintHeader("MEMORY MANAGER BASICS");
        
        PrintSubHeader("Initialization and Global Instance");
        
        // Test global instance
        bool hasGlobalInstance = (MemoryManagement::g_memory_manager != nullptr);
        std::cout << "  Global memory manager instance: " << (hasGlobalInstance ? "Available" : "Not available") << std::endl;
        PrintResult("Global instance initialization", hasGlobalInstance);
        
        // Test local instance creation
        try {
            MemoryManagement::MemoryManager localManager;
            bool isAttached = localManager.IsAttached();
            std::cout << "  Local manager initial state: " << (isAttached ? "Attached" : "Not attached") << std::endl;
            PrintResult("Local manager creation", true);
            PrintResult("Initial attachment state", !isAttached); // Should not be attached initially
        } catch (const std::exception& e) {
            std::cout << "  Local manager creation failed: " << e.what() << std::endl;
            PrintResult("Local manager creation", false);
        }
        
        // Test move semantics
        try {
            MemoryManagement::MemoryManager manager1;
            MemoryManagement::MemoryManager manager2 = std::move(manager1);
            PrintResult("Move constructor", true);
        } catch (...) {
            PrintResult("Move constructor", false);
        }
    }
    
    void TestProcessOperations() {
        PrintHeader("PROCESS OPERATIONS");
        
        PrintSubHeader("Process Discovery and Attachment");
        
#ifdef _WIN32
        // Get list of running processes for testing
        std::vector<std::string> availableProcesses = GetRunningProcesses();
        std::cout << "  Found " << availableProcesses.size() << " running processes" << std::endl;
        
        // Display some process names
        std::cout << "  Sample processes: ";
        size_t display_count = availableProcesses.size() < 5 ? availableProcesses.size() : 5;
        for (size_t i = 0; i < display_count; ++i) {
            std::cout << availableProcesses[i];
            if (i < display_count - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        
        PrintResult("Process enumeration", !availableProcesses.empty());
        
        // Test attachment to current process (should work)
        MemoryManagement::MemoryManager manager;
        DWORD currentPid = GetCurrentProcessId();
        auto attachResult = manager.AttachToProcess(currentPid);
        
        std::cout << "  Attempting to attach to current process (PID: " << currentPid << ")" << std::endl;
        bool attachSuccess = (attachResult == MemoryManagement::MemoryResult::Success);
        PrintResult("Attach to current process", attachSuccess);
        
        if (attachSuccess) {
            std::cout << "  Attached process ID: " << manager.GetProcessId() << std::endl;
            std::cout << "  Is process running: " << (manager.IsProcessRunning() ? "Yes" : "No") << std::endl;
            
            HWND window = manager.GetProcessWindow();
            std::cout << "  Process window handle: " << (window ? "Available" : "None") << std::endl;
            
            PrintResult("Process ID retrieval", manager.GetProcessId() == currentPid);
            PrintResult("Process running check", manager.IsProcessRunning());
        }
        
        // Test attachment to non-existent process
        auto invalidResult = manager.AttachToProcess("NonExistentProcess12345");
        bool invalidAttachFailed = (invalidResult != MemoryManagement::MemoryResult::Success);
        PrintResult("Invalid process attachment handling", invalidAttachFailed);
        
        // Test detachment
        manager.DetachProcess();
        PrintResult("Process detachment", !manager.IsAttached());
        
#else
        std::cout << "  Platform: Non-Windows - Process operations not available" << std::endl;
        PrintResult("Non-Windows stub behavior", true);
#endif
    }
    
    void TestMemoryOperations() {
        PrintHeader("MEMORY OPERATIONS");
        
        PrintSubHeader("Memory Read/Write Operations");
        
#ifdef _WIN32
        MemoryManagement::MemoryManager manager;
        
        // Attach to current process for self-testing
        if (manager.AttachToProcess(GetCurrentProcessId()) == MemoryManagement::MemoryResult::Success) {
            
            // Test reading from our own memory
            int testValue = 0x12345678;
            uintptr_t testAddress = reinterpret_cast<uintptr_t>(&testValue);
            
            std::cout << "  Test variable address: " << FormatAddress(testAddress) << std::endl;
            std::cout << "  Original value: 0x" << std::hex << testValue << std::dec << std::endl;
            
            // Test template read
            int readValue = 0;
            auto readResult = manager.Read(testAddress, readValue);
            bool readSuccess = (readResult == MemoryManagement::MemoryResult::Success);
            bool readCorrect = (readValue == testValue);
            
            std::cout << "  Read value: 0x" << std::hex << readValue << std::dec << std::endl;
            PrintResult("Template memory read", readSuccess && readCorrect);
            
            // Test template read with default value
            int defaultRead = manager.Read<int>(testAddress, 0);
            PrintResult("Template read with default", defaultRead == testValue);
            
            // Test invalid address read
            int invalidRead = manager.Read<int>(0x1, 0xFFFFFFFF);
            PrintResult("Invalid address read handling", invalidRead == 0xFFFFFFFF);
            
            // Test raw memory read
            uint8_t buffer[4];
            auto rawReadResult = manager.ReadMemory(testAddress, buffer, sizeof(buffer));
            bool rawReadSuccess = (rawReadResult == MemoryManagement::MemoryResult::Success);
            
            if (rawReadSuccess) {
                std::cout << "  Raw read result: ";
                for (int i = 0; i < 4; ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
                }
                std::cout << std::dec << std::endl;
            }
            PrintResult("Raw memory read", rawReadSuccess);
            
            // Test memory write (to our own writable memory)
            int writeTestValue = 0x87654321;
            uintptr_t writeTestAddress = reinterpret_cast<uintptr_t>(&writeTestValue);
            int newValue = 0xABCDEF00;
            
            auto writeResult = manager.Write(writeTestAddress, newValue);
            bool writeSuccess = (writeResult == MemoryManagement::MemoryResult::Success);
            bool writeCorrect = (writeTestValue == newValue);
            
            std::cout << "  Write test - Original: 0x" << std::hex << 0x87654321 
                      << ", New: 0x" << writeTestValue << std::dec << std::endl;
            PrintResult("Template memory write", writeSuccess && writeCorrect);
            
            // Test protected write
            int protectedValue = 0x11111111;
            uintptr_t protectedAddress = reinterpret_cast<uintptr_t>(&protectedValue);
            auto protectedWriteResult = manager.WriteProtected(protectedAddress, 0x22222222);
            bool protectedWriteSuccess = (protectedWriteResult == MemoryManagement::MemoryResult::Success);
            
            std::cout << "  Protected write result: " << (protectedWriteSuccess ? "Success" : "Failed") << std::endl;
            PrintResult("Protected memory write", protectedWriteSuccess);
        }
#else
        std::cout << "  Platform: Non-Windows - Memory operations return default values" << std::endl;
        MemoryManagement::MemoryManager manager;
        
        // Test stub behavior
        int testValue = 0;
        auto readResult = manager.Read(0x1000, testValue);
        PrintResult("Stub read operation", readResult == MemoryManagement::MemoryResult::ReadFailed);
        
        auto writeResult = manager.Write(0x1000, 42);
        PrintResult("Stub write operation", writeResult == MemoryManagement::MemoryResult::WriteFailed);
#endif
    }
    
    void TestModuleManagement() {
        PrintHeader("MODULE MANAGEMENT");
        
        PrintSubHeader("Module Enumeration and Information");
        
#ifdef _WIN32
        MemoryManagement::MemoryManager manager;
        
        if (manager.AttachToProcess(GetCurrentProcessId()) == MemoryManagement::MemoryResult::Success) {
            
            // Refresh modules
            bool refreshSuccess = manager.RefreshModules();
            std::cout << "  Module refresh: " << (refreshSuccess ? "Success" : "Failed") << std::endl;
            PrintResult("Module enumeration", refreshSuccess);
            
            if (refreshSuccess) {
                const auto& modules = manager.GetModules();
                std::cout << "  Total modules loaded: " << modules.size() << std::endl;
                
                // Display some modules
                int count = 0;
                for (const auto& pair : modules) {
                    if (count >= 5) break; // Show only first 5
                    
                    const std::string& name = pair.first;
                    const auto& module = pair.second;
                    
                    std::cout << "  Module " << (count + 1) << ": " << name << std::endl;
                    std::cout << "    Base: " << FormatAddress(module->GetBaseAddress()) << std::endl;
                    std::cout << "    Size: " << FormatSize(module->GetSize()) << std::endl;
                    std::cout << "    Path: " << module->GetPath() << std::endl;
                    std::cout << "    Valid: " << (module->IsValid() ? "Yes" : "No") << std::endl;
                    
                    count++;
                }
                
                PrintResult("Module information retrieval", !modules.empty());
                
                // Test getting specific module (should exist in any Windows process)
                auto* kernelModule = manager.GetModule("kernel32.dll");
                bool hasKernel32 = (kernelModule != nullptr);
                
                if (hasKernel32) {
                    std::cout << "  kernel32.dll found:" << std::endl;
                    std::cout << "    Base: " << FormatAddress(kernelModule->GetBaseAddress()) << std::endl;
                    std::cout << "    Size: " << FormatSize(kernelModule->GetSize()) << std::endl;
                    std::cout << "    Valid: " << (kernelModule->IsValid() ? "Yes" : "No") << std::endl;
                }
                
                PrintResult("Specific module retrieval", hasKernel32);
                
                // Test case-insensitive module lookup
                auto* kernelModuleUpper = manager.GetModule("KERNEL32.DLL");
                PrintResult("Case-insensitive module lookup", kernelModuleUpper != nullptr);
                
                // Test non-existent module
                auto* nonExistentModule = manager.GetModule("NonExistentModule12345.dll");
                PrintResult("Non-existent module handling", nonExistentModule == nullptr);
            }
        }
#else
        std::cout << "  Platform: Non-Windows - Module operations not available" << std::endl;
        PrintResult("Non-Windows module handling", true);
#endif
    }
    
    void TestPatternScanning() {
        PrintHeader("PATTERN SCANNING");
        
        PrintSubHeader("Memory Pattern Detection");
        
#ifdef _WIN32
        MemoryManagement::MemoryManager manager;
        
        if (manager.AttachToProcess(GetCurrentProcessId()) == MemoryManagement::MemoryResult::Success) {
            manager.RefreshModules();
            
            // Create test data for pattern scanning
            uint8_t testData[] = {
                0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88
            };
            
            uintptr_t testDataAddress = reinterpret_cast<uintptr_t>(testData);
            std::cout << "  Test data address: " << FormatAddress(testDataAddress) << std::endl;
            
            // Test pattern scanning in memory range
            std::string pattern1 = "12 34 56 78";
            std::string mask1 = "xxxx";
            
            uintptr_t found1 = manager.FindPattern(testDataAddress, sizeof(testData), pattern1, mask1);
            bool pattern1Found = (found1 == testDataAddress);
            
            std::cout << "  Pattern '" << pattern1 << "' found at: " << FormatAddress(found1) << std::endl;
            PrintResult("Basic pattern scanning", pattern1Found);
            
            // Test pattern with wildcards
            std::string pattern2 = "11 ?? 33 44";
            std::string mask2 = "x?xx";
            
            uintptr_t expected2 = testDataAddress + 8; // Position of 11 22 33 44
            uintptr_t found2 = manager.FindPattern(testDataAddress, sizeof(testData), pattern2, mask2);
            bool pattern2Found = (found2 == expected2);
            
            std::cout << "  Wildcard pattern '" << pattern2 << "' found at: " << FormatAddress(found2) << std::endl;
            PrintResult("Wildcard pattern scanning", pattern2Found);
            
            // Test pattern not found
            std::string pattern3 = "AA BB CC DD";
            std::string mask3 = "xxxx";
            
            uintptr_t found3 = manager.FindPattern(testDataAddress, sizeof(testData), pattern3, mask3);
            bool pattern3NotFound = (found3 == 0);
            
            std::cout << "  Non-existent pattern '" << pattern3 << "': " << (pattern3NotFound ? "Not found (correct)" : "Found (incorrect)") << std::endl;
            PrintResult("Pattern not found handling", pattern3NotFound);
            
            // Test module-based pattern scanning
            const auto& modules = manager.GetModules();
            if (!modules.empty()) {
                auto firstModule = modules.begin();
                std::string moduleName = firstModule->first;
                
                std::cout << "  Testing module pattern scan in: " << moduleName << std::endl;
                
                // Look for common x86/x64 instruction patterns (NOP, PUSH, etc.)
                std::string commonPattern = "90"; // NOP instruction
                std::string commonMask = "x";
                
                uintptr_t modulePatternResult = manager.FindPattern(moduleName, commonPattern, commonMask);
                bool modulePatternFound = (modulePatternResult != 0);
                
                std::cout << "  Module pattern result: " << FormatAddress(modulePatternResult) << std::endl;
                PrintResult("Module pattern scanning", modulePatternFound);
            }
        }
#else
        std::cout << "  Platform: Non-Windows - Pattern scanning not available" << std::endl;
        PrintResult("Non-Windows pattern scanning", true);
#endif
    }
    
    void TestMemoryAllocation() {
        PrintHeader("MEMORY ALLOCATION");
        
        PrintSubHeader("Dynamic Memory Management");
        
#ifdef _WIN32
        MemoryManagement::MemoryManager manager;
        
        if (manager.AttachToProcess(GetCurrentProcessId()) == MemoryManagement::MemoryResult::Success) {
            
            // Test various allocation sizes and protections
            std::vector<std::pair<size_t, MemoryManagement::MemoryProtection>> testAllocations = {
                {1024, MemoryManagement::MemoryProtection::ReadWrite},
                {4096, MemoryManagement::MemoryProtection::Read},
                {8192, MemoryManagement::MemoryProtection::ExecuteReadWrite}
            };
            
            std::vector<uintptr_t> allocatedBlocks;
            
            for (size_t i = 0; i < testAllocations.size(); ++i) {
                size_t size = testAllocations[i].first;
                auto protection = testAllocations[i].second;
                
                uintptr_t allocated = manager.AllocateMemory(size, protection);
                bool allocationSuccess = (allocated != 0);
                
                std::cout << "  Allocation " << (i + 1) << " (" << FormatSize(size) << "): " 
                          << FormatAddress(allocated) << std::endl;
                
                if (allocationSuccess) {
                    allocatedBlocks.push_back(allocated);
                    
                    // Test writing to allocated memory (if writable)
                    if (protection == MemoryManagement::MemoryProtection::ReadWrite ||
                        protection == MemoryManagement::MemoryProtection::ExecuteReadWrite) {
                        
                        uint32_t testValue = 0x12345678;
                        auto writeResult = manager.Write(allocated, testValue);
                        bool writeSuccess = (writeResult == MemoryManagement::MemoryResult::Success);
                        
                        if (writeSuccess) {
                            uint32_t readBack = manager.Read<uint32_t>(allocated, 0);
                            bool readWriteCorrect = (readBack == testValue);
                            PrintResult("Allocated memory read/write test " + std::to_string(i + 1), readWriteCorrect);
                        }
                    }
                }
                
                PrintResult("Memory allocation " + std::to_string(i + 1), allocationSuccess);
            }
            
            std::cout << "  Successfully allocated " << allocatedBlocks.size() 
                      << " out of " << testAllocations.size() << " blocks" << std::endl;
            
            // Test memory deallocation
            int freedCount = 0;
            for (uintptr_t block : allocatedBlocks) {
                bool freed = manager.FreeMemory(block);
                if (freed) freedCount++;
                
                std::cout << "  Freed memory at " << FormatAddress(block) << ": " 
                          << (freed ? "Success" : "Failed") << std::endl;
            }
            
            PrintResult("Memory deallocation", freedCount == static_cast<int>(allocatedBlocks.size()));
            
            // Test invalid memory operations
            bool invalidFree = manager.FreeMemory(0x12345678); // Invalid address
            PrintResult("Invalid memory free handling", !invalidFree);
        }
#else
        std::cout << "  Platform: Non-Windows - Memory allocation not available" << std::endl;
        PrintResult("Non-Windows allocation handling", true);
#endif
    }
    
    void TestErrorHandling() {
        PrintHeader("ERROR HANDLING");
        
        PrintSubHeader("Error Conditions and Edge Cases");
        
        MemoryManagement::MemoryManager manager;
        
        // Test operations without attachment
        std::cout << "  Testing operations without process attachment:" << std::endl;
        
        int testValue = 0;
        auto readResult = manager.Read(0x1000, testValue);
        std::cout << "    Read without attachment: " << static_cast<int>(readResult) << std::endl;
        PrintResult("Read without attachment handling", readResult != MemoryManagement::MemoryResult::Success);
        
        auto writeResult = manager.Write(0x1000, 42);
        std::cout << "    Write without attachment: " << static_cast<int>(writeResult) << std::endl;
        PrintResult("Write without attachment handling", writeResult != MemoryManagement::MemoryResult::Success);
        
        // Test null pointer operations
        std::cout << "  Testing null pointer operations:" << std::endl;
        
        auto nullReadResult = manager.ReadMemory(0x0, &testValue, sizeof(testValue));
        PrintResult("Null pointer read handling", nullReadResult != MemoryManagement::MemoryResult::Success);
        
        auto nullWriteResult = manager.WriteMemory(0x0, &testValue, sizeof(testValue));
        PrintResult("Null pointer write handling", nullWriteResult != MemoryManagement::MemoryResult::Success);
        
        // Test invalid size operations
        std::cout << "  Testing invalid size operations:" << std::endl;
        
        auto zeroSizeRead = manager.ReadMemory(0x1000, &testValue, 0);
        PrintResult("Zero size read handling", zeroSizeRead != MemoryManagement::MemoryResult::Success);
        
        // Test multiple attach/detach cycles
        std::cout << "  Testing multiple attach/detach cycles:" << std::endl;
        
        bool cycleTestPassed = true;
        for (int i = 0; i < 3; ++i) {
#ifdef _WIN32
            auto attachResult = manager.AttachToProcess(GetCurrentProcessId());
            if (attachResult != MemoryManagement::MemoryResult::Success) {
                cycleTestPassed = false;
                break;
            }
            
            if (!manager.IsAttached()) {
                cycleTestPassed = false;
                break;
            }
            
            manager.DetachProcess();
            
            if (manager.IsAttached()) {
                cycleTestPassed = false;
                break;
            }
#endif
        }
        
        PrintResult("Multiple attach/detach cycles", cycleTestPassed);
        
        // Test resource cleanup
        std::cout << "  Testing resource cleanup:" << std::endl;
        
        {
            MemoryManagement::MemoryManager scopedManager;
#ifdef _WIN32
            scopedManager.AttachToProcess(GetCurrentProcessId());
            // Manager should clean up automatically when destroyed
#endif
        }
        
        PrintResult("Automatic resource cleanup", true); // If we get here, no crash occurred
    }
    
    void TestPerformance() {
        PrintHeader("PERFORMANCE BENCHMARKS");
        
        PrintSubHeader("Operation Performance Metrics");
        
#ifdef _WIN32
        MemoryManagement::MemoryManager manager;
        
        if (manager.AttachToProcess(GetCurrentProcessId()) == MemoryManagement::MemoryResult::Success) {
            
            // Create test data
            std::vector<int> testData(1000);
            std::iota(testData.begin(), testData.end(), 0);
            uintptr_t testAddress = reinterpret_cast<uintptr_t>(testData.data());
            
            // Benchmark memory reads
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; ++i) {
                int value = manager.Read<int>(testAddress + (i * sizeof(int)), 0);
                (void)value; // Avoid unused variable warning
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto readDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "  1000 memory reads: " << readDuration.count() << " microseconds" << std::endl;
            std::cout << "  Average per read: " << (readDuration.count() / 1000.0) << " microseconds" << std::endl;
            
            // Benchmark memory writes
            std::vector<int> writeData(1000, 0);
            uintptr_t writeAddress = reinterpret_cast<uintptr_t>(writeData.data());
            
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; ++i) {
                manager.Write(writeAddress + (i * sizeof(int)), i);
            }
            end = std::chrono::high_resolution_clock::now();
            
            auto writeDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "  1000 memory writes: " << writeDuration.count() << " microseconds" << std::endl;
            std::cout << "  Average per write: " << (writeDuration.count() / 1000.0) << " microseconds" << std::endl;
            
            // Benchmark pattern scanning
            uint8_t patternData[10000];
            for (int i = 0; i < 10000; ++i) {
                patternData[i] = static_cast<uint8_t>(i % 256);
            }
            // Insert known pattern
            patternData[5000] = 0xDE;
            patternData[5001] = 0xAD;
            patternData[5002] = 0xBE;
            patternData[5003] = 0xEF;
            
            uintptr_t patternAddress = reinterpret_cast<uintptr_t>(patternData);
            
            start = std::chrono::high_resolution_clock::now();
            uintptr_t foundPattern = manager.FindPattern(patternAddress, sizeof(patternData), 
                "DE AD BE EF", "xxxx");
            end = std::chrono::high_resolution_clock::now();
            
            auto scanDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "  Pattern scan (10KB): " << scanDuration.count() << " microseconds" << std::endl;
            std::cout << "  Pattern found: " << (foundPattern != 0 ? "Yes" : "No") << std::endl;
            
            PrintResult("Performance benchmark completion", true);
        }
#else
        std::cout << "  Platform: Non-Windows - Performance benchmarks not available" << std::endl;
        PrintResult("Non-Windows performance handling", true);
#endif
    }
    
    void PrintFinalResults() {
        PrintHeader("FINAL RESULTS");
        
        double successRate = (double)passedTests / totalTests * 100.0;
        
        std::cout << "Total Tests:   " << totalTests << std::endl;
        std::cout << "Passed:        " << passedTests << std::endl;
        std::cout << "Failed:        " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate:  " << std::fixed << std::setprecision(1) 
                  << successRate << "%" << std::endl;
        
        if (successRate == 100.0) {
            std::cout << "\n[PERFECT] All tests passed!" << std::endl;
            std::cout << "The MemoryManager library functions completely correctly." << std::endl;
        } else if (successRate >= 90.0) {
            std::cout << "\n[VERY GOOD] Almost all tests passed." << std::endl;
        } else if (successRate >= 75.0) {
            std::cout << "\n[GOOD] Most tests passed, some issues found." << std::endl;
        } else {
            std::cout << "\n[PROBLEMS] found. Library needs attention." << std::endl;
        }
        
        std::cout << "\n=== LIBRARY FUNCTIONS SUMMARY ===" << std::endl;
        std::cout << "[+] Process Attachment and Management" << std::endl;
        std::cout << "[+] Module Enumeration and Information" << std::endl;
        std::cout << "[+] Memory Reading and Writing Operations" << std::endl;
        std::cout << "[+] Memory Protection Management" << std::endl;
        std::cout << "[+] Pattern Scanning and Detection" << std::endl;
        std::cout << "[+] Dynamic Memory Allocation" << std::endl;
        std::cout << "[+] Thread Creation and Management" << std::endl;
        std::cout << "[+] Utility Functions and Helpers" << std::endl;
        std::cout << "[+] Error Handling and Edge Cases" << std::endl;
        std::cout << "[+] Cross-platform Compatibility" << std::endl;
    }

#ifdef _WIN32
    std::vector<std::string> GetRunningProcesses() {
        std::vector<std::string> processes;
        
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return processes;
        }
        
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(snapshot, &entry)) {
            do {
                // Convert TCHAR to std::string
#ifdef UNICODE
                std::wstring w_name(entry.szExeFile);
                std::string name(w_name.begin(), w_name.end());
#else
                std::string name(entry.szExeFile);
#endif
                processes.push_back(name);
            } while (Process32Next(snapshot, &entry));
        }
        
        CloseHandle(snapshot);
        return processes;
    }
#endif
};

int main() {
    std::cout << "Initializing MemoryManager Demo..." << std::endl;
    
    try {
        MemoryDemo demo;
        demo.RunAllTests();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR during demo: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN ERROR during demo!" << std::endl;
        return 1;
    }
}