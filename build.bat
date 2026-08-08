@echo off
setlocal EnableDelayedExpansion

set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%
set PROJECT_NAME=SoftwareRasterizer

set PLATFORM=%1
set BUILD_TYPE=%2
set PROFILE_FLAG=
set SIMD_FLAG=

if "%PLATFORM%"=="" goto usage
if "%PLATFORM%"=="--help" goto usage
if "%PLATFORM%"=="-h" goto usage
if "%PLATFORM%"=="help" goto usage

if "%PLATFORM%"=="init" goto init

if "%PLATFORM%"=="win" goto build

echo Error: Unknown platform '%PLATFORM%'. >&2
goto usage

:usage
echo Usage:
echo   build.bat init
echo   build.bat win debug [--profile]
echo   build.bat win release [--profile] [--simd]
echo.
echo Examples:
echo   build.bat init
echo   build.bat win debug
echo   build.bat win release --simd
echo   build.bat win release --simd --profile
exit /b 1

:init
echo Initializing git submodules...
git -C "%PROJECT_ROOT%" submodule update --init --recursive
if errorlevel 1 (
    echo Error: Failed to initialize submodules. >&2
    exit /b 1
)
echo Submodules initialized successfully.
exit /b 0

:build
if "%BUILD_TYPE%"=="" (
    echo Error: Build type argument required. >&2
    goto usage
)
if not "%BUILD_TYPE%"=="debug" if not "%BUILD_TYPE%"=="release" (
    echo Error: Invalid build type '%BUILD_TYPE%'. Expected 'debug' or 'release'. >&2
    exit /b 1
)
for %%f in (%3 %4) do (
    if "%%f"=="--profile" ( set PROFILE_FLAG=--profile
    ) else if "%%f"=="--simd" ( set SIMD_FLAG=--simd
    ) else (
        echo Error: unknown flag '%%f'. >&2
        goto usage
    )
)

if "%SIMD_FLAG%"=="--simd" if "%BUILD_TYPE%"=="debug" (
    echo Error: --simd is not supported for debug builds. Use: build.bat win release --simd >&2
    exit /b 1
)

if not exist "%PROJECT_ROOT%\vendors\SDL3\CMakeLists.txt" (
    echo Error: SDL3 submodule not found. Run 'build.bat init' first. >&2
    exit /b 1
)

if "%BUILD_TYPE%"=="debug" (
    set PRESET=windows-debug
    set BUILD_DIR=%PROJECT_ROOT%\build\debug
) else (
    set PRESET=windows-release
    set BUILD_DIR=%PROJECT_ROOT%\build\release
)

set EXECUTABLE=%BUILD_DIR%\%PROJECT_NAME%.exe

set PROFILING_CMAKE_FLAG=-DENABLE_PROFILING=OFF
if "%PROFILE_FLAG%"=="--profile" set PROFILING_CMAKE_FLAG=-DENABLE_PROFILING=ON

set SIMD_CMAKE_FLAG=-DENABLE_SIMD=OFF
if "%SIMD_FLAG%"=="--simd" set SIMD_CMAKE_FLAG=-DENABLE_SIMD=ON

echo Configuring Windows %BUILD_TYPE% build...
cmake --preset %PRESET% -S "%PROJECT_ROOT%" %PROFILING_CMAKE_FLAG% %SIMD_CMAKE_FLAG%
if errorlevel 1 (
    echo Error: CMake configuration failed. >&2
    exit /b 1
)

echo Building Windows %BUILD_TYPE%...
cmake --build --preset %PRESET% --target %PROJECT_NAME% --parallel
if errorlevel 1 (
    echo Error: Build failed. >&2
    exit /b 1
)

if not exist "%EXECUTABLE%" (
    echo Error: Build completed, but the executable was not found at: %EXECUTABLE% >&2
    exit /b 1
)

echo.
echo Build completed successfully.
echo Executable: %EXECUTABLE%
echo.
echo Running %PROJECT_NAME%...
echo ----------------------------------------

if "%PROFILE_FLAG%"=="--profile" (
    if exist "%PROJECT_ROOT%\trace.json" del /f "%PROJECT_ROOT%\trace.json"
)

"%EXECUTABLE%"
