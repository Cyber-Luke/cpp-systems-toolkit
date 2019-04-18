@echo off
echo Building and running ProcessManager library demo...
echo Platform: Windows (Full functionality)
echo Compiler: MSVC C++17

:: Change to the parent directory (main project directory)
cd /d "%~dp0.."

echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Compiling process manager demo executable...
cl /EHsc /std:c++17 /O2 ^
   examples/process_manager_demo.cpp ^
   libraries/process-tools/ProcessManager.cpp ^
   /Fe:compiled/process_manager_demo.exe ^
   kernel32.lib user32.lib advapi32.lib psapi.lib

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo Running process manager demo...
echo.

compiled\process_manager_demo.exe

echo.
echo Demo execution completed.