# Process Management Library

A comprehensive C++ library for Windows process inspection, memory management, and inter-process operations. Designed for system administration, debugging, forensics, and educational purposes.

## Features

- **Process Discovery**: Find and enumerate running processes
- **Module Inspection**: List and analyze loaded modules
- **Memory Operations**: Type-safe memory read/write operations
- **Pattern Scanning**: Binary pattern matching in process memory
- **Memory Protection**: Modify memory region protections
- **Thread Management**: Remote thread creation and enumeration
- **RAII Design**: Automatic resource cleanup

## Usage Examples

### Basic Process Attachment

```cpp
#include "ProcessManager.hpp"

ProcessManager pm;

// Attach to a process by name
if (pm.AttachToProcess("notepad.exe")) {
    std::cout << "Successfully attached to notepad.exe" << std::endl;

    // Get process information
    DWORD pid = pm.GetProcessId();
    std::string processPath = pm.GetProcessPath();

    // Detach when done
    pm.DetachFromProcess();
}
```

### RAII Process Management

```cpp
// Automatic attachment and cleanup
{
    ScopedProcessAttachment attachment(pm, "target.exe");
    if (attachment) {
        // Process operations here
        // Automatically detached when leaving scope
    }
}
```

### Module Enumeration

```cpp
// List all modules in the process
auto modules = pm.EnumerateModules();
for (const auto& module : modules) {
    std::cout << "Module: " << module.GetName()
              << " at 0x" << std::hex << module.GetBaseAddress()
              << " (Size: " << module.GetSize() << " bytes)" << std::endl;
}

// Get specific module information
ModuleInfo kernel32 = pm.GetModule("kernel32.dll");
if (kernel32.IsValid()) {
    std::uintptr_t baseAddr = kernel32.GetBaseAddress();
}
```

### Memory Operations

```cpp
// Type-safe memory operations
int value = 42;
std::uintptr_t address = 0x1000000;

// Write memory
pm.WriteMemory(address, value);

// Read memory
int readValue = pm.ReadMemory<int>(address);

// Read into buffer
int buffer;
if (pm.ReadMemory(address, buffer)) {
    std::cout << "Read value: " << buffer << std::endl;
}

// Memory region operations
std::vector<uint8_t> data(1024);
pm.ReadMemoryRegion(address, data.data(), data.size());
```

### Pattern Scanning

```cpp
// Scan for binary patterns
std::string pattern = "\\x48\\x89\\x5C\\x24\\x08";
std::string mask = "xxxxx";

std::uintptr_t result = pm.PatternScan(pattern, mask);
if (result != 0) {
    std::cout << "Pattern found at: 0x" << std::hex << result << std::endl;
}

// Scan within specific module
std::uintptr_t moduleResult = pm.PatternScanModule("user32.dll", pattern, mask);
```

### Process Discovery

```cpp
// Get all running processes
auto processes = ProcessManager::GetRunningProcesses();
for (const auto& proc : processes) {
    std::cout << "Process: " << proc << std::endl;
}

// Check if specific process is running
if (ProcessManager::IsProcessRunning("chrome.exe")) {
    std::cout << "Chrome is running" << std::endl;
}

// Find process ID
DWORD pid = ProcessManager::FindProcessId("explorer.exe");
```

### Memory Protection

```cpp
// Change memory protection
DWORD oldProtection;
bool success = pm.ChangeMemoryProtection(address, 4096, PAGE_EXECUTE_READWRITE, &oldProtection);

if (success) {
    // Perform operations on executable memory

    // Restore original protection
    pm.ChangeMemoryProtection(address, 4096, oldProtection);
}
```

### Advanced Operations

```cpp
// Allocate memory in target process
std::uintptr_t allocatedMemory = pm.AllocateMemory(4096);
if (allocatedMemory != 0) {
    // Use allocated memory

    // Free when done
    pm.FreeMemory(allocatedMemory);
}

// Create remote thread
HANDLE remoteThread = pm.CreateRemoteThread(functionAddress, parameter);
if (remoteThread != INVALID_HANDLE_VALUE) {
    WaitForSingleObject(remoteThread, INFINITE);
    CloseHandle(remoteThread);
}
```

## Error Handling

```cpp
// Get detailed error information
if (!pm.AttachToProcess("nonexistent.exe")) {
    std::string errorMsg = ProcessManager::GetLastErrorString();
    std::cerr << "Failed to attach: " << errorMsg << std::endl;
}
```

## Safety Considerations

- Always check return values for success/failure
- Use RAII wrappers for automatic cleanup
- Verify process architecture compatibility
- Handle access denied scenarios gracefully
- Test with appropriate privileges

## Dependencies

- Windows API (windows.h, tlhelp32.h, psapi.h)
- C++11 or later
- Administrative privileges may be required for some operations
