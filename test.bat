@echo off
setlocal EnableDelayedExpansion

set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%
set BUILD_DIR=%PROJECT_ROOT%\build\debug
set TEST_BINARY=%BUILD_DIR%\SoftwareRasterizerTests.exe

if not exist "%PROJECT_ROOT%\vendors\SDL3\CMakeLists.txt" (
    echo Error: SDL3 submodule not found. Run 'build.bat init' first. >&2
    exit /b 1
)

cmake --preset windows-debug -S "%PROJECT_ROOT%" --log-level=WARNING
if errorlevel 1 (
    echo Error: CMake configuration failed. >&2
    exit /b 1
)

cmake --build --preset windows-debug --target SoftwareRasterizerTests --parallel
if errorlevel 1 (
    echo Error: Build failed. >&2
    exit /b 1
)

echo.
echo Running tests...
echo ----------------------------------------

"%TEST_BINARY%"
