@echo off
REM Build script for CryptoUtils Demo
REM Comprehensive demonstration of the CryptoUtils library

echo ==========================================
echo CRYPTOUTILS DEMO - BUILD SCRIPT
echo ==========================================
echo.
echo Compiles and runs the comprehensive demo
echo of the CryptoUtils library with all functions
echo.

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
)
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
)
if errorlevel 1 (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
)

REM Check if compiler is available
cl >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Visual Studio compiler not found!
    echo Please install Visual Studio 2019 or 2022 with C++ support
    echo.
    pause
    exit /b 1
)

REM Create output directory
if not exist "compiled" mkdir compiled

echo [INFO] Compiling crypto_demo.exe...
echo.

REM Compiler flags for optimized release build
set CPPFLAGS=/std:c++17 /EHsc /O2 /W3 /DUNICODE /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /DWIN32_LEAN_AND_MEAN /DNOMINMAX /DNDEBUG
set INCLUDES=/I"libraries"
set LIBS=advapi32.lib kernel32.lib user32.lib

REM Compile the demo
cl %CPPFLAGS% %INCLUDES% examples\crypto_demo.cpp libraries\crypto-utils\CryptoUtils.cpp /Fe:compiled\crypto_demo.exe %LIBS%

if errorlevel 1 (
    echo.
    echo [ERROR] Failed to build crypto_demo.exe
    echo Check the error messages above for details
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo [SUCCESS] crypto_demo.exe built successfully!
    echo.
    echo Executable location: compiled\crypto_demo.exe
    echo File size: 
    for %%I in (compiled\crypto_demo.exe) do echo   %%~zI bytes
    echo.
    echo ==========================================
    echo AUTO-STARTING DEMO...
    echo ==========================================
    echo.
    
    REM Auto-start the demo
    compiled\crypto_demo.exe
    
    echo.
    echo ==========================================
    echo DEMO COMPLETED
    echo ==========================================
)