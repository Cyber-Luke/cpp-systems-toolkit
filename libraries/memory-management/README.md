# Memory Management Library

A comprehensive Windows process memory management library for educational purposes, system administration, and debugging applications.

## Features

- **Process Attachment**: Attach to processes by name or ID with proper access control
- **Memory Operations**: Type-safe memory reading and writing with error handling
- **Module Management**: Enumerate and manage loaded modules with caching
- **Pattern Scanning**: Advanced pattern searching with byte masking support
- **Memory Protection**: Change memory protection attributes safely
- **Remote Thread Creation**: Create and manage threads in target processes
- **Memory Allocation**: Allocate and free memory in remote processes

## Classes

### MemoryManager

The main class providing all memory management functionality.

#### Basic Usage

```cpp
#include "libraries/memory-management/MemoryManager.hpp"

MemoryManager memMgr;

// Attach to a process
if (memMgr.AttachToProcess("notepad.exe")) {
    // Read memory
    int value;
    if (memMgr.ReadMemory(0x12345678, value)) {
        std::cout << "Read value: " << value << std::endl;
    }
    
    // Write memory
    int newValue = 42;
    if (memMgr.WriteMemory(0x12345678, newValue)) {
        std::cout << "Successfully wrote value" << std::endl;
    }
    
    // Detach when done
    memMgr.DetachFromProcess();
}
```

#### Advanced Pattern Scanning

```cpp
// Find a pattern in a specific module
uintptr_t address = memMgr.PatternScanModule(
    "kernel32.dll",
    "\x48\x8B\x05\x00\x00\x00\x00",  // Pattern bytes
    "xxx????"                         // Mask: x = match, ? = wildcard
);

if (address != 0) {
    std::cout << "Pattern found at: 0x" << std::hex << address << std::endl;
}
```

#### Module Enumeration

```cpp
auto modules = memMgr.EnumerateModules();
for (const auto& module : modules) {
    std::cout << "Module: " << module.GetName() 
              << " Base: 0x" << std::hex << module.GetBaseAddress()
              << " Size: " << module.GetSize() << std::endl;
}
```

### ProcessModule

Container class for module information.

```cpp
ProcessModule module = memMgr.GetModule("user32.dll");
if (module.IsValid()) {
    uintptr_t base = module.GetBaseAddress();
    size_t size = module.GetSize();
    std::string name = module.GetName();
    std::string path = module.GetPath();
}
```

### MemoryProtectionGuard

RAII wrapper for automatic memory protection management.

```cpp
{
    MemoryProtectionGuard guard(memMgr, address, size, MemoryProtection::ExecuteReadWrite);
    if (guard.IsValid()) {
        // Memory protection is temporarily changed
        // Write to normally protected memory
        memMgr.WriteMemory(address, someData);
    }
    // Protection is automatically restored when guard goes out of scope
}
```

### Utility Functions

```cpp
// Global memory manager
MemoryManagement::InitializeMemoryManager();
auto& globalMgr = *MemoryManagement::g_memory_manager;

// Utility functions
bool isValid = MemoryManagement::Utils::IsValidAddress(0x12345678);
uintptr_t base = MemoryManagement::Utils::GetModuleBase("kernel32.dll");
```

## Memory Operations

### Template-based Type Safety

```cpp
// Read different data types safely
float health = memMgr.Read<float>(playerHealthAddress);
Vec3 position = memMgr.Read<Vec3>(playerPositionAddress);
char playerName[32];
memMgr.ReadMemory(playerNameAddress, playerName, sizeof(playerName));
```

### Protection Management

```cpp
DWORD oldProtection;
bool success = memMgr.ChangeMemoryProtection(
    address, 
    size, 
    PAGE_EXECUTE_READWRITE, 
    &oldProtection
);
```

### Remote Memory Allocation

```cpp
// Allocate memory in target process
uintptr_t allocatedMemory = memMgr.AllocateMemory(
    1024,                           // Size in bytes
    MEM_COMMIT | MEM_RESERVE,      // Allocation type
    PAGE_EXECUTE_READWRITE         // Protection
);

if (allocatedMemory != 0) {
    // Use the allocated memory
    memMgr.WriteMemory(allocatedMemory, someData);
    
    // Free when done
    memMgr.FreeMemory(allocatedMemory);
}
```

## Pattern Scanning

### Basic Pattern Matching

```cpp
// Simple pattern: look for specific byte sequence
uintptr_t result = memMgr.PatternScan(
    "\x55\x8B\xEC",    // push ebp; mov ebp, esp
    "xxx"              // All bytes must match exactly
);
```

### Advanced Pattern with Wildcards

```cpp
// Complex pattern with wildcards
uintptr_t result = memMgr.PatternScan(
    "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0",  // mov rax, [rip+????]; test rax, rax
    "xxx????xxx"                                    // Wildcards for the displacement
);
```

## Error Handling

All memory operations return appropriate error codes or boolean success indicators:

```cpp
if (!memMgr.IsAttached()) {
    std::cerr << "Not attached to any process" << std::endl;
    return;
}

if (!memMgr.IsProcessRunning()) {
    std::cerr << "Target process is no longer running" << std::endl;
    memMgr.DetachFromProcess();
    return;
}
```

## Platform Support

- **Windows**: Full functionality with Windows API integration
- **Other Platforms**: Stub implementations for compilation compatibility

## Security Considerations

- Always run with appropriate privileges for process access
- Respect process security boundaries and user permissions
- Use for educational and legitimate debugging purposes only
- Handle sensitive memory data appropriately

## Thread Safety

The MemoryManager class is not thread-safe. Use appropriate synchronization when accessing from multiple threads.

## Performance Notes

- Module information is cached after first access
- Pattern scanning uses optimized algorithms for large memory regions
- Batch operations are available for improved performance
- Consider memory protection overhead when writing frequently

## Example: Complete Memory Scanner

```cpp
#include "libraries/memory-management/MemoryManager.hpp"
#include <iostream>

int main() {
    MemoryManagement::InitializeMemoryManager();
    auto& memMgr = *MemoryManagement::g_memory_manager;
    
    // Attach to target process
    if (!memMgr.AttachToProcess("target.exe")) {
        std::cerr << "Failed to attach to process" << std::endl;
        return 1;
    }
    
    std::cout << "Attached to process ID: " << memMgr.GetProcessId() << std::endl;
    
    // Find a specific pattern
    uintptr_t patternAddress = memMgr.PatternScanModule(
        "main.dll",
        "\x48\x89\x5C\x24\x08",  // mov [rsp+8], rbx
        "xxxxx"
    );
    
    if (patternAddress != 0) {
        std::cout << "Pattern found at: 0x" << std::hex << patternAddress << std::endl;
        
        // Read some data near the pattern
        int32_t value = memMgr.Read<int32_t>(patternAddress + 0x10);
        std::cout << "Value at offset +0x10: " << value << std::endl;
    }
    
    // Cleanup
    memMgr.DetachFromProcess();
    MemoryManagement::CleanupMemoryManager();
    
    return 0;
}
```

## Build Requirements

- Windows SDK (for Windows-specific functionality)
- C++17 or later
- PSAPI library (automatically linked on Windows)

## Educational Use

This library is designed for:
- Learning system programming concepts
- Understanding process memory management
- Developing debugging and analysis tools
- Reverse engineering education
- Security research (ethical use only)

Always use responsibly and in accordance with applicable laws and ethical guidelines.