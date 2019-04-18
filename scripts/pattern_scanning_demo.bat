@echo off
echo Building and running PatternScanning library demo...
echo Platform: Windows (Full functionality)
echo Compiler: MSVC C++17

:: Change to the parent directory (main project directory)
cd /d "%~dp0.."

echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Compiling pattern scanning demo executable...
cl /EHsc /std:c++17 /O2 ^
   examples/pattern_scanning_demo.cpp ^
   libraries/pattern-scanning/PatternScanning.cpp ^
   /Fe:compiled/pattern_scanning_demo.exe ^
   kernel32.lib user32.lib advapi32.lib

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo Running pattern scanning demo...
echo.

compiled\pattern_scanning_demo.exe

echo.
echo Demo execution completed.
del *.obj >nul 2>&1