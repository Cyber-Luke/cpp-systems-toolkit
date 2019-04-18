/**
 * @file MemoryManager.cpp
 * @brief Implementation of memory management library for Windows processes
 * @author Lukas Ernst
 * 
 * Implementation of process memory operations including secure memory access,
 * module enumeration, pattern scanning, and process management. Provides
 * Windows-specific optimizations and error handling for educational purposes.
 */

#ifdef _WIN32

#include "MemoryManager.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>

namespace MemoryManagement {

// ----------------------------------------------
// Global instance
// ----------------------------------------------
std::unique_ptr<MemoryManager> g_memory_manager = nullptr;

void InitializeMemoryManager() {
    if (!g_memory_manager) {
        g_memory_manager = std::make_unique<MemoryManager>();
    }
}

void CleanupMemoryManager() {
    if (g_memory_manager) {
        g_memory_manager->DetachProcess();
        g_memory_manager.reset();
    }
}

// ----------------------------------------------
// Utility helpers (internal)
// ----------------------------------------------
namespace {
    DWORD ToWinProtect(MemoryProtection prot) {
        switch (prot) {
        case MemoryProtection::Read: return PAGE_READONLY;
        case MemoryProtection::ReadWrite: return PAGE_READWRITE;
        case MemoryProtection::Execute: return PAGE_EXECUTE;
        case MemoryProtection::ExecuteRead: return PAGE_EXECUTE_READ;
        case MemoryProtection::ExecuteReadWrite: return PAGE_EXECUTE_READWRITE;
        default: return PAGE_NOACCESS;
        }
    }

    std::string ToLower(const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
        return out;
    }

    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == static_cast<DWORD>(lParam)) {
            // Basic heuristics: visible top-level window
            if (IsWindowVisible(hwnd)) {
                SetLastError(ERROR_SUCCESS);
                *((HWND*)(&lParam)) = hwnd; // store in same memory (workaround to avoid extra struct)
                return FALSE; // stop enumeration
            }
        }
        return TRUE;
    }
}

// ----------------------------------------------
// MemoryManager move semantics
// ----------------------------------------------
MemoryManager::MemoryManager(MemoryManager&& other) noexcept {
    process_handle_ = other.process_handle_;
    process_id_ = other.process_id_;
    process_window_ = other.process_window_;
    modules_ = std::move(other.modules_);
    allocated_memory_ = std::move(other.allocated_memory_);

    other.process_handle_ = nullptr;
    other.process_id_ = 0;
    other.process_window_ = nullptr;
}

MemoryManager& MemoryManager::operator=(MemoryManager&& other) noexcept {
    if (this != &other) {
        DetachProcess();
        process_handle_ = other.process_handle_;
        process_id_ = other.process_id_;
        process_window_ = other.process_window_;
        modules_ = std::move(other.modules_);
        allocated_memory_ = std::move(other.allocated_memory_);

        other.process_handle_ = nullptr;
        other.process_id_ = 0;
        other.process_window_ = nullptr;
    }
    return *this;
}

// ----------------------------------------------
// Process attachment
// ----------------------------------------------
MemoryResult MemoryManager::AttachToProcess(const std::string& process_name) {
    DWORD pid = FindProcessId(process_name);
    if (!pid) return MemoryResult::ProcessNotFound;
    return AttachToProcess(pid);
}

MemoryResult MemoryManager::AttachToProcess(DWORD process_id) {
    DetachProcess();
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, FALSE, process_id);
    if (!h) {
        return (GetLastError() == ERROR_ACCESS_DENIED) ? MemoryResult::AccessDenied : MemoryResult::ProcessNotFound;
    }
    process_handle_ = h;
    process_id_ = process_id;
    EnumerateModules();
    return MemoryResult::Success;
}

void MemoryManager::DetachProcess() {
    if (process_handle_) {
        // Free any tracked allocations (best-effort)
        for (auto addr : allocated_memory_) {
            VirtualFreeEx(process_handle_, reinterpret_cast<LPVOID>(addr), 0, MEM_RELEASE);
        }
        allocated_memory_.clear();
        CloseHandle(process_handle_);
        process_handle_ = nullptr;
    }
    process_id_ = 0;
    process_window_ = nullptr;
    modules_.clear();
}

bool MemoryManager::IsProcessRunning() const {
    if (!process_handle_) return false;
    DWORD code = 0;
    if (!GetExitCodeProcess(process_handle_, &code)) return false;
    return code == STILL_ACTIVE;
}

HWND MemoryManager::GetProcessWindow() const {
    if (process_window_ && IsWindow(process_window_)) return process_window_;
    if (!process_id_) return nullptr;

    // Use a lambda-friendly structure
    struct Data { DWORD pid; HWND hwnd; } data{ process_id_, nullptr };
    EnumWindows([](HWND hwnd, LPARAM lParam)->BOOL {
        auto* d = reinterpret_cast<Data*>(lParam);
        DWORD pid = 0; GetWindowThreadProcessId(hwnd, &pid);
        if (pid == d->pid && IsWindowVisible(hwnd)) { d->hwnd = hwnd; return FALSE; }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));
    process_window_ = data.hwnd;
    return process_window_;
}

// ----------------------------------------------
// Memory operations
// ----------------------------------------------
MemoryResult MemoryManager::ReadMemory(uintptr_t address, void* buffer, size_t size) const {
    if (!process_handle_) return MemoryResult::ProcessNotFound;
    if (!Utils::IsValidAddress(address)) return MemoryResult::InvalidAddress;
    SIZE_T bytes_read = 0;
    if (!ReadProcessMemory(process_handle_, reinterpret_cast<LPCVOID>(address), buffer, size, &bytes_read) || bytes_read != size) {
        return MemoryResult::ReadFailed;
    }
    return MemoryResult::Success;
}

MemoryResult MemoryManager::WriteMemory(uintptr_t address, const void* data, size_t size) {
    if (!process_handle_) return MemoryResult::ProcessNotFound;
    SIZE_T bytes_written = 0;
    if (!WriteProcessMemory(process_handle_, reinterpret_cast<LPVOID>(address), data, size, &bytes_written) || bytes_written != size) {
        return MemoryResult::WriteFailed;
    }
    return MemoryResult::Success;
}

MemoryResult MemoryManager::WriteMemoryProtected(uintptr_t address, const void* data, size_t size) {
    if (!process_handle_) return MemoryResult::ProcessNotFound;
    DWORD oldProt = 0;
    // Use VirtualProtectEx directly so we can restore any Windows protection value (not only those mapped in enum)
    if (!VirtualProtectEx(process_handle_, reinterpret_cast<LPVOID>(address), size, PAGE_READWRITE, &oldProt)) {
        return MemoryResult::ProtectionFailed;
    }
    MemoryResult res = WriteMemory(address, data, size);
    DWORD unused;
    VirtualProtectEx(process_handle_, reinterpret_cast<LPVOID>(address), size, oldProt, &unused); // best-effort restore
    return res;
}

MemoryResult MemoryManager::ChangeProtection(uintptr_t address, size_t size, MemoryProtection new_protection, DWORD* old_protection) {
    if (!process_handle_) return MemoryResult::ProcessNotFound;
    DWORD oldProtLocal = 0;
    if (!VirtualProtectEx(process_handle_, reinterpret_cast<LPVOID>(address), size, ToWinProtect(new_protection), &oldProtLocal)) {
        return MemoryResult::ProtectionFailed;
    }
    if (old_protection) *old_protection = oldProtLocal;
    return MemoryResult::Success;
}

// ----------------------------------------------
// Module handling
// ----------------------------------------------
bool MemoryManager::EnumerateModules() {
    modules_.clear();
    if (!process_handle_) return false;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id_);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    MODULEENTRY32 me{}; me.dwSize = sizeof(me);
    if (Module32First(snapshot, &me)) {
        do {
            // Convert TCHAR to std::string for both UNICODE and ANSI builds
#ifdef UNICODE
            std::wstring w_name(me.szModule);
            std::string name(w_name.begin(), w_name.end());
            std::wstring w_path(me.szExePath);
            std::string path(w_path.begin(), w_path.end());
#else
            std::string name(me.szModule);
            std::string path(me.szExePath);
#endif
            auto key = ToLower(name);
            modules_[key] = std::make_unique<ProcessModule>(reinterpret_cast<uintptr_t>(me.modBaseAddr), me.modBaseSize, name, path);
        } while (Module32Next(snapshot, &me));
    }
    CloseHandle(snapshot);
    return !modules_.empty();
}

bool MemoryManager::RefreshModules() { return EnumerateModules(); }

ProcessModule* MemoryManager::GetModule(const std::string& module_name) {
    auto it = modules_.find(ToLower(module_name));
    if (it != modules_.end()) return it->second.get();
    // Attempt refresh once
    if (EnumerateModules()) {
        it = modules_.find(ToLower(module_name));
        if (it != modules_.end()) return it->second.get();
    }
    return nullptr;
}

// ----------------------------------------------
// Pattern scanning
// ----------------------------------------------
uintptr_t MemoryManager::PatternScan(const uint8_t* data, size_t data_size, const std::string& pattern, const std::string& mask) {
    // Convert human-readable pattern ("48 8B ?? 0D") into bytes with 0 placeholders for wildcards
    auto pattern_bytes = Utils::PatternToBytes(pattern);
    if (pattern_bytes.empty() || mask.empty() || pattern_bytes.size() != mask.size()) return SIZE_MAX;
    size_t pattern_len = pattern_bytes.size();
    if (pattern_len > data_size) return SIZE_MAX;
    for (size_t i = 0; i <= data_size - pattern_len; ++i) {
        bool match = true;
        for (size_t j = 0; j < pattern_len; ++j) {
            if (mask[j] == 'x' && data[i + j] != pattern_bytes[j]) { match = false; break; }
        }
        if (match) return i; // valid even if 0
    }
    return SIZE_MAX;
}

uintptr_t MemoryManager::FindPattern(const std::string& module_name, const std::string& pattern, const std::string& mask) {
    auto* mod = GetModule(module_name);
    if (!mod || !mod->IsValid()) return 0;
    std::vector<uint8_t> buffer(mod->GetSize());
    if (ReadMemory(mod->GetBaseAddress(), buffer.data(), buffer.size()) != MemoryResult::Success) return 0;
    uintptr_t offset = PatternScan(buffer.data(), buffer.size(), pattern, mask);
    if (offset == SIZE_MAX) return 0;
    return mod->GetBaseAddress() + offset;
}

uintptr_t MemoryManager::FindPattern(uintptr_t start_address, size_t search_size, const std::string& pattern, const std::string& mask) {
    std::vector<uint8_t> buffer(search_size);
    if (ReadMemory(start_address, buffer.data(), buffer.size()) != MemoryResult::Success) return 0;
    uintptr_t offset = PatternScan(buffer.data(), buffer.size(), pattern, mask);
    if (offset == SIZE_MAX) return 0;
    return start_address + offset;
}

// ----------------------------------------------
// Remote thread & memory allocation
// ----------------------------------------------
HANDLE MemoryManager::CreateRemoteThread(uintptr_t address, void* parameter) {
    if (!process_handle_) return nullptr;
    return ::CreateRemoteThread(process_handle_, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), parameter, 0, nullptr);
}

uintptr_t MemoryManager::AllocateMemory(size_t size, MemoryProtection protection) {
    if (!process_handle_) return 0;
    LPVOID mem = VirtualAllocEx(process_handle_, nullptr, size, MEM_COMMIT | MEM_RESERVE, ToWinProtect(protection));
    if (!mem) return 0;
    uintptr_t addr = reinterpret_cast<uintptr_t>(mem);
    allocated_memory_.push_back(addr);
    return addr;
}

bool MemoryManager::FreeMemory(uintptr_t address) {
    if (!process_handle_ || !address) return false;
    if (VirtualFreeEx(process_handle_, reinterpret_cast<LPVOID>(address), 0, MEM_RELEASE)) {
        allocated_memory_.erase(std::remove(allocated_memory_.begin(), allocated_memory_.end(), address), allocated_memory_.end());
        return true;
    }
    return false;
}

// ----------------------------------------------
// Static helpers
// ----------------------------------------------
DWORD MemoryManager::FindProcessId(const std::string& process_name) {
    std::string target = ToLower(process_name);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32 pe{}; pe.dwSize = sizeof(pe);
    DWORD pid = 0;
    if (Process32First(snapshot, &pe)) {
        do {
            // Convert TCHAR to std::string for both UNICODE and ANSI builds
#ifdef UNICODE
            std::wstring w_exe(pe.szExeFile);
            std::string exe = ToLower(std::string(w_exe.begin(), w_exe.end()));
#else
            std::string exe = ToLower(std::string(pe.szExeFile));
#endif
            if (exe == target) { pid = pe.th32ProcessID; break; }
        } while (Process32Next(snapshot, &pe));
    }
    CloseHandle(snapshot);
    return pid;
}

// ----------------------------------------------
// MemoryProtectionGuard
// ----------------------------------------------
MemoryProtectionGuard::MemoryProtectionGuard(MemoryManager& memory_mgr, uintptr_t address, size_t size, MemoryProtection new_protection)
    : memory_manager_(memory_mgr), address_(address), size_(size), old_protection_(0), is_valid_(false) {
    if (memory_manager_.IsAttached()) {
        if (memory_manager_.ChangeProtection(address_, size_, new_protection, &old_protection_) == MemoryResult::Success) {
            is_valid_ = true;
        }
    }
}

MemoryProtectionGuard::~MemoryProtectionGuard() {
    if (is_valid_) {
        memory_manager_.ChangeProtection(address_, size_, static_cast<MemoryProtection>(old_protection_), nullptr);
    }
}

// ----------------------------------------------
// Utils implementations
// ----------------------------------------------
namespace Utils {

std::vector<uint8_t> PatternToBytes(const std::string& pattern) {
    std::vector<uint8_t> bytes;
    std::istringstream iss(pattern);
    std::string token;
    while (iss >> token) {
        if (token == "??" || token == "?") {
            bytes.push_back(0x00); // placeholder
        } else {
            bytes.push_back(static_cast<uint8_t>(std::strtoul(token.c_str(), nullptr, 16)));
        }
    }
    return bytes;
}

std::string CreateMaskFromPattern(const std::string& pattern) {
    std::ostringstream mask;
    std::istringstream iss(pattern);
    std::string token;
    while (iss >> token) {
        if (token == "??" || token == "?") mask << '?'; else mask << 'x';
    }
    return mask.str();
}

bool IsValidAddress(uintptr_t address) {
    if (!address) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    // We can only validate inside current process reliably without handle; assume true otherwise.
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        DWORD protect = mbi.Protect;
        if (protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
        return true;
    }
    return false; // Fallback conservative
}

uintptr_t GetModuleBase(const std::string& module_name) {
    if (!g_memory_manager || !g_memory_manager->IsAttached()) return 0;
    auto* mod = g_memory_manager->GetModule(module_name);
    return mod ? mod->GetBaseAddress() : 0;
}

uintptr_t CalculateRelativeAddress(uintptr_t base_address, ptrdiff_t offset) {
    return base_address + offset;
}

} // namespace Utils

} // namespace MemoryManagement

#else // Non-Windows stub implementations

// Provide minimal stubs so code can compile on non-Windows (no functionality).
#include "MemoryManager.hpp"
namespace MemoryManagement {
std::unique_ptr<MemoryManager> g_memory_manager = nullptr;
void InitializeMemoryManager() {}
void CleanupMemoryManager() {}
}

#endif // _WIN32
