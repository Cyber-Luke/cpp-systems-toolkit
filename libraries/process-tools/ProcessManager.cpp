/**
 * @file ProcessManager.cpp
 * @brief Implementation of advanced process management and inspection utilities
 * @author Lukas Ernst
 * 
 * Implementation of process inspection, memory management, and inter-process
 * communication features. Provides Windows-specific optimizations and secure
 * process handling for educational and debugging purposes.
 */

#include "ProcessManager.hpp"
#include <sstream>
#include <iomanip>

// ProcessManager Implementation

ProcessManager::ProcessManager() 
    : m_processHandle(INVALID_HANDLE_VALUE), m_processId(0), m_isAttached(false) {
}

ProcessManager::~ProcessManager() {
    DetachFromProcess();
}

bool ProcessManager::AttachToProcess(const std::string& processName) {
    DWORD processId = FindProcessId(processName);
    if (processId == 0) {
        return false;
    }
    return AttachToProcessById(processId);
}

bool ProcessManager::AttachToProcessById(DWORD processId) {
    DetachFromProcess(); // Detach from any existing process

    m_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (m_processHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    m_processId = processId;
    m_isAttached = true;
    ClearCache();
    return true;
}

void ProcessManager::DetachFromProcess() {
    if (m_processHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_processHandle);
        m_processHandle = INVALID_HANDLE_VALUE;
    }
    m_processId = 0;
    m_isAttached = false;
    ClearCache();
}

std::vector<std::string> ProcessManager::GetRunningProcesses() {
    std::vector<std::string> processes;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        do {
            // Convert to std::string (handle both ANSI and UNICODE builds)
#ifdef UNICODE
            std::wstring w_exe(processEntry.szExeFile);
            std::string exe(w_exe.begin(), w_exe.end());
#else
            std::string exe(processEntry.szExeFile);
#endif
            processes.emplace_back(exe);
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return processes;
}

DWORD ProcessManager::FindProcessId(const std::string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    DWORD processId = 0;
    if (Process32First(snapshot, &processEntry)) {
        do {
            // Convert to std::string for comparison (handle both ANSI and UNICODE builds)
#ifdef UNICODE
            std::wstring w_exe(processEntry.szExeFile);
            std::string exe(w_exe.begin(), w_exe.end());
#else
            std::string exe(processEntry.szExeFile);
#endif
            if (processName == exe) {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return processId;
}

bool ProcessManager::IsProcessRunning(const std::string& processName) {
    return FindProcessId(processName) != 0;
}

std::vector<ModuleInfo> ProcessManager::EnumerateModules() {
    std::vector<ModuleInfo> modules;
    
    if (!m_isAttached) {
        return modules;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_processId);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return modules;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            // Convert to std::string (handle both ANSI and UNICODE builds)
#ifdef UNICODE
            std::wstring w_name(moduleEntry.szModule);
            std::string name(w_name.begin(), w_name.end());
            std::wstring w_path(moduleEntry.szExePath);
            std::string path(w_path.begin(), w_path.end());
#else
            std::string name(moduleEntry.szModule);
            std::string path(moduleEntry.szExePath);
#endif
            
            modules.emplace_back(
                reinterpret_cast<std::uintptr_t>(moduleEntry.modBaseAddr),
                moduleEntry.modBaseSize,
                name,
                path
            );
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return modules;
}

ModuleInfo ProcessManager::GetModule(const std::string& moduleName) {
    // Check cache first
    auto it = m_moduleCache.find(moduleName);
    if (it != m_moduleCache.end()) {
        return it->second;
    }

    // Search for module
    auto modules = EnumerateModules();
    for (const auto& module : modules) {
        if (module.GetName() == moduleName) {
            m_moduleCache[moduleName] = module;
            return module;
        }
    }

    return ModuleInfo(0, 0, "", ""); // Invalid module
}

std::uintptr_t ProcessManager::GetModuleBase(const std::string& moduleName) {
    return GetModule(moduleName).GetBaseAddress();
}

bool ProcessManager::IsModuleLoaded(const std::string& moduleName) {
    return GetModule(moduleName).IsValid();
}

void ProcessManager::ClearCache() {
    m_moduleCache.clear();
}

std::string ProcessManager::GetLastErrorString() {
    DWORD error = GetLastError();
    LPSTR messageBuffer = nullptr;
    
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        nullptr
    );
    
    std::string message(messageBuffer);
    LocalFree(messageBuffer);
    return message;
}

// Helper function to convert wide string to string
std::string WideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Implementation of missing ProcessManager methods

bool ProcessManager::ReadMemoryRegion(std::uintptr_t address, void* buffer, std::size_t size) {
    if (!m_isAttached || !buffer) return false;
    
    SIZE_T bytesRead;
    return ReadProcessMemory(m_processHandle, 
                           reinterpret_cast<LPCVOID>(address), 
                           buffer, 
                           size, 
                           &bytesRead) && (bytesRead == size);
}

bool ProcessManager::WriteMemoryRegion(std::uintptr_t address, const void* data, std::size_t size) {
    if (!m_isAttached || !data) return false;
    
    SIZE_T bytesWritten;
    return WriteProcessMemory(m_processHandle, 
                            reinterpret_cast<LPVOID>(address), 
                            data, 
                            size, 
                            &bytesWritten) && (bytesWritten == size);
}

bool ProcessManager::ChangeMemoryProtection(std::uintptr_t address, std::size_t size, DWORD newProtection, DWORD* oldProtection) {
    if (!m_isAttached) return false;
    
    DWORD oldProt;
    bool result = VirtualProtectEx(m_processHandle, 
                                  reinterpret_cast<LPVOID>(address), 
                                  size, 
                                  newProtection, 
                                  &oldProt) != FALSE;
    
    if (oldProtection) {
        *oldProtection = oldProt;
    }
    
    return result;
}

MEMORY_BASIC_INFORMATION ProcessManager::QueryMemoryRegion(std::uintptr_t address) {
    MEMORY_BASIC_INFORMATION mbi = {};
    
    if (m_isAttached) {
        VirtualQueryEx(m_processHandle, 
                      reinterpret_cast<LPCVOID>(address), 
                      &mbi, 
                      sizeof(mbi));
    }
    
    return mbi;
}

std::vector<DWORD> ProcessManager::GetThreadIds() {
    std::vector<DWORD> threadIds;
    
    if (!m_isAttached) return threadIds;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return threadIds;
    }
    
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    
    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == m_processId) {
                threadIds.push_back(threadEntry.th32ThreadID);
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    
    CloseHandle(snapshot);
    return threadIds;
}

HANDLE ProcessManager::CreateRemoteThread(std::uintptr_t startAddress, void* parameter) {
    if (!m_isAttached) return INVALID_HANDLE_VALUE;
    
    return ::CreateRemoteThread(m_processHandle, 
                               nullptr, 
                               0, 
                               reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress), 
                               parameter, 
                               0, 
                               nullptr);
}

std::uintptr_t ProcessManager::PatternScan(const std::string& pattern, const std::string& mask, std::uintptr_t startAddress, std::size_t searchSize) {
    if (!m_isAttached || pattern.empty() || mask.empty() || pattern.length() != mask.length()) {
        return 0;
    }
    
    // If no specific range provided, scan all readable memory
    if (startAddress == 0 || searchSize == 0) {
        MEMORY_BASIC_INFORMATION mbi;
        std::uintptr_t address = 0;
        
        while (VirtualQueryEx(m_processHandle, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect & PAGE_GUARD) == 0 && 
                (mbi.Protect & PAGE_READONLY || mbi.Protect & PAGE_READWRITE || 
                 mbi.Protect & PAGE_EXECUTE_READ || mbi.Protect & PAGE_EXECUTE_READWRITE)) {
                
                std::vector<uint8_t> buffer(mbi.RegionSize);
                SIZE_T bytesRead;
                
                if (ReadProcessMemory(m_processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                    std::uintptr_t result = PatternScanInBuffer(buffer.data(), bytesRead, pattern, mask);
                    if (result != 0) {
                        return reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + result;
                    }
                }
            }
            
            address = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        }
    } else {
        // Scan specific range
        std::vector<uint8_t> buffer(searchSize);
        SIZE_T bytesRead;
        
        if (ReadProcessMemory(m_processHandle, reinterpret_cast<LPCVOID>(startAddress), 
                             buffer.data(), searchSize, &bytesRead)) {
            std::uintptr_t result = PatternScanInBuffer(buffer.data(), bytesRead, pattern, mask);
            if (result != 0) {
                return startAddress + result;
            }
        }
    }
    
    return 0;
}

std::uintptr_t ProcessManager::PatternScanModule(const std::string& moduleName, const std::string& pattern, const std::string& mask) {
    ModuleInfo module = GetModule(moduleName);
    if (!module.IsValid()) {
        return 0;
    }
    
    return PatternScan(pattern, mask, module.GetBaseAddress(), module.GetSize());
}

bool ProcessManager::IsProcessArchitectureMatch() {
    if (!m_isAttached) return false;
    
    BOOL isWow64Process = FALSE;
    BOOL isCurrentWow64 = FALSE;
    
    IsWow64Process(m_processHandle, &isWow64Process);
    IsWow64Process(GetCurrentProcess(), &isCurrentWow64);
    
    return isWow64Process == isCurrentWow64;
}

// Helper method for pattern scanning in buffer
std::uintptr_t ProcessManager::PatternScanInBuffer(const uint8_t* buffer, std::size_t bufferSize, 
                                                  const std::string& pattern, const std::string& mask) {
    if (!buffer || bufferSize < pattern.length()) {
        return 0;
    }
    
    const char* patternBytes = pattern.c_str();
    const char* maskBytes = mask.c_str();
    std::size_t patternLength = pattern.length();
    
    for (std::size_t i = 0; i <= bufferSize - patternLength; ++i) {
        bool found = true;
        for (std::size_t j = 0; j < patternLength; ++j) {
            if (maskBytes[j] == 'x' && buffer[i + j] != static_cast<uint8_t>(patternBytes[j])) {
                found = false;
                break;
            }
        }
        
        if (found) {
            return i;
        }
    }
    
    return 0;
}

std::string ProcessManager::GetProcessName() {
    if (!m_isAttached) return "";
    
    WCHAR processName[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(m_processHandle, 0, processName, &size)) {
        std::wstring w_name(processName);
        std::string name = WideStringToString(w_name);
        
        // Extract just the filename
        size_t pos = name.find_last_of("\\/");
        if (pos != std::string::npos) {
            return name.substr(pos + 1);
        }
        return name;
    }
    return "";
}

std::string ProcessManager::GetProcessPath() {
    if (!m_isAttached) return "";
    
    WCHAR processPath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(m_processHandle, 0, processPath, &size)) {
        std::wstring w_path(processPath);
        return WideStringToString(w_path);
    }
    return "";
}

std::uintptr_t ProcessManager::AllocateMemory(std::size_t size, DWORD allocationType, DWORD protection) {
    if (!m_isAttached) return 0;
    
    LPVOID address = VirtualAllocEx(m_processHandle, nullptr, size, allocationType, protection);
    return reinterpret_cast<std::uintptr_t>(address);
}

bool ProcessManager::FreeMemory(std::uintptr_t address, std::size_t size, DWORD freeType) {
    if (!m_isAttached) return false;
    
    return VirtualFreeEx(m_processHandle, reinterpret_cast<LPVOID>(address), size, freeType) != FALSE;
}