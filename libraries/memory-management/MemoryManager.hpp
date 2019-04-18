/**
 * @file MemoryManager.hpp
 * @brief Professional memory management library for Windows processes
 * @author Lukas Ernst
 * 
 * Provides safe and efficient process memory operations including process attachment,
 * module enumeration, memory reading/writing with protection handling, pattern scanning,
 * and memory allocation. Designed for educational purposes and systems programming.
 */

#pragma once

// This header provides a Windows process memory management interface.
// On non-Windows platforms only lightweight no-op stubs are provided so the
// project can still compile (the functionality is Windows-specific).

#ifdef _WIN32

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

/**
 * @file MemoryManager.hpp
 * @brief Professional memory management library for Windows processes
 * 
 * Provides safe and efficient process memory operations including:
 * - Process attachment and detachment
 * - Module enumeration and management  
 * - Safe memory reading and writing with protection handling
 * - Pattern scanning and signature detection
 * - Memory allocation and thread creation
 * 
 * This library is designed for educational purposes and systems programming.
 */

namespace MemoryManagement {

/**
 * @brief Module information container
 */
class ProcessModule {
public:
    ProcessModule() = default;
    
    ProcessModule(uintptr_t base, size_t size, const std::string& name, const std::string& path)
        : base_address_(base), module_size_(size), module_name_(name), module_path_(path) {}

    uintptr_t GetBaseAddress() const { return base_address_; }
    size_t GetSize() const { return module_size_; }
    const std::string& GetName() const { return module_name_; }
    const std::string& GetPath() const { return module_path_; }
    
    bool IsValid() const { return base_address_ != 0 && module_size_ > 0; }

private:
    uintptr_t base_address_ = 0;
    size_t module_size_ = 0;
    std::string module_name_;
    std::string module_path_;
};

/**
 * @brief Memory protection and access management
 */
enum class MemoryProtection : DWORD {
    None = 0,
    Read = PAGE_READONLY,
    ReadWrite = PAGE_READWRITE,
    Execute = PAGE_EXECUTE,
    ExecuteRead = PAGE_EXECUTE_READ,
    ExecuteReadWrite = PAGE_EXECUTE_READWRITE
};

/**
 * @brief Memory operation result codes
 */
enum class MemoryResult {
    Success,
    ProcessNotFound,
    AccessDenied,
    InvalidAddress,
    ReadFailed,
    WriteFailed,
    ProtectionFailed
};

/**
 * @brief Main memory manager class for process operations
 */
class MemoryManager {
public:
    MemoryManager() = default;
    ~MemoryManager() { DetachProcess(); }

    // No copy construction/assignment
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    // Move construction/assignment
    MemoryManager(MemoryManager&& other) noexcept;
    MemoryManager& operator=(MemoryManager&& other) noexcept;

    /**
     * @brief Attach to a process by name
     */
    MemoryResult AttachToProcess(const std::string& process_name);
    
    /**
     * @brief Attach to a process by ID
     */
    MemoryResult AttachToProcess(DWORD process_id);

    /**
     * @brief Detach from current process
     */
    void DetachProcess();

    /**
     * @brief Check if currently attached to a process
     */
    bool IsAttached() const { return process_handle_ != nullptr; }

    /**
     * @brief Check if the attached process is still running
     */
    bool IsProcessRunning() const;

    /**
     * @brief Get the current process ID
     */
    DWORD GetProcessId() const { return process_id_; }

    /**
     * @brief Get process window handle
     */
    HWND GetProcessWindow() const;

    // Memory operations
    template<typename T>
    MemoryResult Read(uintptr_t address, T& buffer) const {
        return ReadMemory(address, &buffer, sizeof(T));
    }

    template<typename T>
    T Read(uintptr_t address, const T& default_value = T{}) const {
        T result;
        if (ReadMemory(address, &result, sizeof(T)) == MemoryResult::Success) {
            return result;
        }
        return default_value;
    }

    template<typename T>
    MemoryResult Write(uintptr_t address, const T& value) {
        return WriteMemory(address, &value, sizeof(T));
    }

    template<typename T>
    MemoryResult WriteProtected(uintptr_t address, const T& value) {
        return WriteMemoryProtected(address, &value, sizeof(T));
    }

    /**
     * @brief Read raw memory block
     */
    MemoryResult ReadMemory(uintptr_t address, void* buffer, size_t size) const;

    /**
     * @brief Write raw memory block
     */
    MemoryResult WriteMemory(uintptr_t address, const void* data, size_t size);

    /**
     * @brief Write memory with protection changes
     */
    MemoryResult WriteMemoryProtected(uintptr_t address, const void* data, size_t size);

    /**
     * @brief Change memory protection
     */
    MemoryResult ChangeProtection(uintptr_t address, size_t size, MemoryProtection new_protection, DWORD* old_protection = nullptr);

    // Module operations
    /**
     * @brief Get module by name
     */
    ProcessModule* GetModule(const std::string& module_name);

    /**
     * @brief Get all loaded modules
     */
    const std::unordered_map<std::string, std::unique_ptr<ProcessModule>>& GetModules() const { return modules_; }

    /**
     * @brief Refresh module list
     */
    bool RefreshModules();

    // Pattern scanning
    /**
     * @brief Find pattern in module
     */
    uintptr_t FindPattern(const std::string& module_name, const std::string& pattern, const std::string& mask);

    /**
     * @brief Find pattern in memory range
     */
    uintptr_t FindPattern(uintptr_t start_address, size_t search_size, const std::string& pattern, const std::string& mask);

    /**
     * @brief Create remote thread
     */
    HANDLE CreateRemoteThread(uintptr_t address, void* parameter = nullptr);

    /**
     * @brief Allocate memory in target process
     */
    uintptr_t AllocateMemory(size_t size, MemoryProtection protection = MemoryProtection::ReadWrite);

    /**
     * @brief Free allocated memory
     */
    bool FreeMemory(uintptr_t address);

private:
    HANDLE process_handle_ = nullptr;
    DWORD process_id_ = 0;
    mutable HWND process_window_ = nullptr;
    std::unordered_map<std::string, std::unique_ptr<ProcessModule>> modules_;
    std::vector<uintptr_t> allocated_memory_;

    // Helper functions
    static DWORD FindProcessId(const std::string& process_name);
    bool EnumerateModules();
    uintptr_t PatternScan(const uint8_t* data, size_t data_size, const std::string& pattern, const std::string& mask);
};

/**
 * @brief RAII memory protection manager
 */
class MemoryProtectionGuard {
public:
    MemoryProtectionGuard(MemoryManager& memory_mgr, uintptr_t address, size_t size, MemoryProtection new_protection);
    ~MemoryProtectionGuard();

    // No copy/move
    MemoryProtectionGuard(const MemoryProtectionGuard&) = delete;
    MemoryProtectionGuard& operator=(const MemoryProtectionGuard&) = delete;

    bool IsValid() const { return is_valid_; }

private:
    MemoryManager& memory_manager_;
    uintptr_t address_;
    size_t size_;
    DWORD old_protection_;
    bool is_valid_;
};

/**
 * @brief Global memory manager instance
 */
extern std::unique_ptr<MemoryManager> g_memory_manager;

/**
 * @brief Initialize global memory manager
 */
void InitializeMemoryManager();

/**
 * @brief Cleanup global memory manager
 */
void CleanupMemoryManager();

/**
 * @brief Utility functions
 */
namespace Utils {
    /**
     * @brief Convert pattern string to bytes
     */
    std::vector<uint8_t> PatternToBytes(const std::string& pattern);
    
    /**
     * @brief Create mask from pattern
     */
    std::string CreateMaskFromPattern(const std::string& pattern);
    
    /**
     * @brief Check if address is valid
     */
    bool IsValidAddress(uintptr_t address);
    
    /**
     * @brief Get module base address by name
     */
    uintptr_t GetModuleBase(const std::string& module_name);
    
    /**
     * @brief Calculate relative address
     */
    uintptr_t CalculateRelativeAddress(uintptr_t base_address, ptrdiff_t offset);
}

} // namespace MemoryManagement

#else // !_WIN32 ------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>

namespace MemoryManagement {

enum class MemoryResult {
    Success,
    ProcessNotFound,
    AccessDenied,
    InvalidAddress,
    ReadFailed,
    WriteFailed,
    ProtectionFailed
};

class MemoryManager {
public:
    MemoryManager() = default;
    MemoryResult AttachToProcess(const std::string&) { return MemoryResult::ProcessNotFound; }
    MemoryResult AttachToProcess(unsigned long) { return MemoryResult::ProcessNotFound; }
    void DetachProcess() {}
    bool IsAttached() const { return false; }
    bool IsProcessRunning() const { return false; }
    unsigned long GetProcessId() const { return 0; }
    void* GetProcessWindow() const { return nullptr; }
    template<typename T>
    MemoryResult Read(uintptr_t, T&) const { return MemoryResult::ReadFailed; }
    template<typename T>
    T Read(uintptr_t, const T& default_value = T{}) const { return default_value; }
    template<typename T>
    MemoryResult Write(uintptr_t, const T&) { return MemoryResult::WriteFailed; }
    template<typename T>
    MemoryResult WriteProtected(uintptr_t, const T&) { return MemoryResult::WriteFailed; }
};

inline void InitializeMemoryManager() {}
inline void CleanupMemoryManager() {}
extern std::unique_ptr<MemoryManager> g_memory_manager; // defined in cpp stub

namespace Utils {
    inline bool IsValidAddress(uintptr_t) { return false; }
    inline uintptr_t GetModuleBase(const std::string&) { return 0; }
}

} // namespace MemoryManagement

#endif // _WIN32
