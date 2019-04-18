/**
 * @file ProcessManager.inl
 * @brief Template implementations for ProcessManager class
 * @author Systems Programming Toolkit
 * @version 1.0
 * 
 * This file contains template method implementations that must be included
 * in the header for proper template instantiation.
 */

#pragma once

// Template method implementations
template<typename T>
bool ProcessManager::ReadMemory(std::uintptr_t address, T& buffer) {
    if (!m_isAttached) return false;
    
    SIZE_T bytesRead;
    return ReadProcessMemory(m_processHandle, 
                           reinterpret_cast<LPCVOID>(address), 
                           &buffer, 
                           sizeof(T), 
                           &bytesRead) && (bytesRead == sizeof(T));
}

template<typename T>
bool ProcessManager::WriteMemory(std::uintptr_t address, const T& data) {
    if (!m_isAttached) return false;
    
    SIZE_T bytesWritten;
    return WriteProcessMemory(m_processHandle, 
                            reinterpret_cast<LPVOID>(address), 
                            &data, 
                            sizeof(T), 
                            &bytesWritten) && (bytesWritten == sizeof(T));
}

template<typename T>
T ProcessManager::ReadMemory(std::uintptr_t address) {
    T buffer{};
    ReadMemory(address, buffer);
    return buffer;
}