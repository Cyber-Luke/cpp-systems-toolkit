/**
 * @file ProcessManager.hpp
 * @brief Advanced process memory management and inspection utilities
 * @author Lukas Ernst
 * 
 * A comprehensive library for Windows process inspection, memory management,
 * and inter-process communication. Designed for educational purposes,
 * system administration, and debugging applications.
 */

#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

/**
 * @brief Represents information about a loaded module
 */
class ModuleInfo {
private:
    std::uintptr_t m_baseAddress;
    std::size_t m_size;
    std::string m_name;
    std::string m_path;

public:
    // Default constructor for std::unordered_map compatibility
    ModuleInfo() : m_baseAddress(0), m_size(0), m_name(), m_path() {}
    
    ModuleInfo(std::uintptr_t base, std::size_t size, const std::string& name, const std::string& path)
        : m_baseAddress(base), m_size(size), m_name(name), m_path(path) {}

    std::uintptr_t GetBaseAddress() const { return m_baseAddress; }
    std::size_t GetSize() const { return m_size; }
    const std::string& GetName() const { return m_name; }
    const std::string& GetPath() const { return m_path; }

    bool IsValid() const { return m_baseAddress != 0 && m_size > 0; }
};

/**
 * @brief Main class for process management and memory operations
 */
class ProcessManager {
private:
    HANDLE m_processHandle;
    DWORD m_processId;
    std::unordered_map<std::string, ModuleInfo> m_moduleCache;
    bool m_isAttached;

    // Internal helper methods
    bool TakeProcessSnapshot();
    bool TakeModuleSnapshot();
    void ClearCache();
    std::uintptr_t PatternScanInBuffer(const uint8_t* buffer, std::size_t bufferSize, 
                                      const std::string& pattern, const std::string& mask);

public:
    ProcessManager();
    ~ProcessManager();

    // Process management
    bool AttachToProcess(const std::string& processName);
    bool AttachToProcessById(DWORD processId);
    void DetachFromProcess();
    bool IsAttached() const { return m_isAttached; }

    // Process discovery
    static std::vector<std::string> GetRunningProcesses();
    static DWORD FindProcessId(const std::string& processName);
    static bool IsProcessRunning(const std::string& processName);

    // Module management
    std::vector<ModuleInfo> EnumerateModules();
    ModuleInfo GetModule(const std::string& moduleName);
    std::uintptr_t GetModuleBase(const std::string& moduleName);
    bool IsModuleLoaded(const std::string& moduleName);

    // Memory operations (template-based for type safety)
    template<typename T>
    bool ReadMemory(std::uintptr_t address, T& buffer);

    template<typename T>
    bool WriteMemory(std::uintptr_t address, const T& data);

    template<typename T>
    T ReadMemory(std::uintptr_t address);

    // Memory region operations
    bool ReadMemoryRegion(std::uintptr_t address, void* buffer, std::size_t size);
    bool WriteMemoryRegion(std::uintptr_t address, const void* data, std::size_t size);
    
    // Memory protection
    bool ChangeMemoryProtection(std::uintptr_t address, std::size_t size, DWORD newProtection, DWORD* oldProtection = nullptr);
    MEMORY_BASIC_INFORMATION QueryMemoryRegion(std::uintptr_t address);

    // Process information
    DWORD GetProcessId() const { return m_processId; }
    HANDLE GetProcessHandle() const { return m_processHandle; }
    std::string GetProcessName();
    std::string GetProcessPath();

    // Advanced operations
    std::uintptr_t AllocateMemory(std::size_t size, DWORD allocationType = MEM_COMMIT | MEM_RESERVE, DWORD protection = PAGE_EXECUTE_READWRITE);
    bool FreeMemory(std::uintptr_t address, std::size_t size = 0, DWORD freeType = MEM_RELEASE);
    
    // Thread management
    std::vector<DWORD> GetThreadIds();
    HANDLE CreateRemoteThread(std::uintptr_t startAddress, void* parameter = nullptr);

    // Pattern scanning
    std::uintptr_t PatternScan(const std::string& pattern, const std::string& mask, std::uintptr_t startAddress = 0, std::size_t searchSize = 0);
    std::uintptr_t PatternScanModule(const std::string& moduleName, const std::string& pattern, const std::string& mask);

    // Utility functions
    static std::string GetLastErrorString();
    bool IsProcessArchitectureMatch();
};

/**
 * @brief RAII wrapper for automatic process detachment
 */
class ScopedProcessAttachment {
private:
    ProcessManager& m_processManager;
    bool m_wasAttached;

public:
    ScopedProcessAttachment(ProcessManager& pm, const std::string& processName)
        : m_processManager(pm), m_wasAttached(false) {
        m_wasAttached = m_processManager.AttachToProcess(processName);
    }

    ~ScopedProcessAttachment() {
        if (m_wasAttached) {
            m_processManager.DetachFromProcess();
        }
    }

    bool IsAttached() const { return m_wasAttached; }
    operator bool() const { return m_wasAttached; }
};

// Include template implementations
#include "ProcessManager.inl"
