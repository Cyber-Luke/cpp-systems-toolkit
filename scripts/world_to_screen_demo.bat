@echo off
setlocal EnableDelayedExpansion

:: Change to the parent directory (main project directory)
cd /d "%~dp0.."

echo ============================================================
echo Building WorldToScreen Library Demo
echo ============================================================
echo.

set DEMO_NAME=world_to_screen_demo
set SOURCE_FILE=examples\%DEMO_NAME%.cpp
set HEADER_FILE=libraries\world-to-screen\WorldToScreen.hpp
set IMPL_FILE=libraries\world-to-screen\WorldToScreen.cpp
set VECTOR_IMPL_FILE=libraries\vector-math\Vector.cpp

echo Checking required files...
if not exist "%SOURCE_FILE%" (
    echo [ERROR] Source file not found: %SOURCE_FILE%
    pause
    exit /b 1
)

if not exist "%HEADER_FILE%" (
    echo [ERROR] Header file not found: %HEADER_FILE%
    pause
    exit /b 1
)

if not exist "%IMPL_FILE%" (
    echo [ERROR] Implementation file not found: %IMPL_FILE%
    pause
    exit /b 1
)

if not exist "%VECTOR_IMPL_FILE%" (
    echo [ERROR] Vector math implementation not found: %VECTOR_IMPL_FILE%
    pause
    exit /b 1
)

echo [OK] All required files found.
echo.

echo Setting up build environment...
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
    if errorlevel 1 (
        echo [ERROR] Visual Studio build tools not found!
        echo Please install Visual Studio 2019 or 2022 with C++ support.
        pause
        exit /b 1
    )
)

echo [OK] Build environment configured.
echo.

echo Compiling WorldToScreen Demo...
echo Command: cl /EHsc /std:c++17 /O2 /Fe:compiled\%DEMO_NAME%.exe %SOURCE_FILE% %IMPL_FILE% %VECTOR_IMPL_FILE%
echo.

cl /EHsc /std:c++17 /O2 /Fe:compiled\%DEMO_NAME%.exe %SOURCE_FILE% %IMPL_FILE% %VECTOR_IMPL_FILE%

if errorlevel 1 (
    echo.
    echo [FAILED] Compilation failed!
    echo Check the error messages above.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] WorldToScreen Demo compiled successfully!
echo Output: compiled\%DEMO_NAME%.exe
echo.

echo Cleaning up intermediate files...
if exist "*.obj" del "*.obj"
if exist "*.pdb" del "*.pdb"

echo [OK] Build completed.
echo.

echo ============================================================
echo Running WorldToScreen Demo
echo ============================================================
echo.

if exist "compiled\%DEMO_NAME%.exe" (
    compiled\%DEMO_NAME%.exe
    echo.
    echo ============================================================
    echo Demo execution completed.
    echo ============================================================
) else (
    echo [ERROR] Executable not found: compiled\%DEMO_NAME%.exe
)

echo.