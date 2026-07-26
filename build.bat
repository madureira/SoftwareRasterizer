@echo off
setlocal EnableDelayedExpansion

set PROJECT_ROOT=%~dp0
set PROJECT_ROOT=%PROJECT_ROOT:~0,-1%
set PROJECT_NAME=SoftwareRasterizer

set BUILD_TYPE=%1
set PLATFORM=%2

if "%BUILD_TYPE%"=="" goto usage
if "%BUILD_TYPE%"=="--help" goto usage
if "%BUILD_TYPE%"=="-h" goto usage
if "%BUILD_TYPE%"=="help" goto usage

if "%BUILD_TYPE%"=="init" goto init

if "%BUILD_TYPE%"=="debug" goto build
if "%BUILD_TYPE%"=="release" goto build

echo Error: Unknown build target '%BUILD_TYPE%'. >&2
goto usage

:usage
echo Usage:
echo   build.bat init
echo   build.bat debug win
echo   build.bat release win
echo.
echo Examples:
echo   build.bat init
echo   build.bat debug win
echo   build.bat release win
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
if "%PLATFORM%"=="" (
    echo Error: Platform argument required. >&2
    goto usage
)
if not "%PLATFORM%"=="win" (
    echo Error: Unsupported platform '%PLATFORM%'. Expected 'win'. >&2
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

echo Configuring Windows %BUILD_TYPE% build...
cmake --preset %PRESET% -S "%PROJECT_ROOT%"
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

"%EXECUTABLE%"
