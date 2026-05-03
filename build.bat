@echo off
setlocal

title LCDream Build

for %%I in ("%~dp0.") do set "ROOT=%%~fI"
set "EXIT_CODE=0"
set "BUILD_TYPE="
set "FORCE_CONFIGURE=0"
set "VENDOR_WAS_MISSING=0"
set "MISSING_VENDOR=0"

echo ====================
echo Building LCDream ... 
echo ====================
echo.

:parse_args
if "%~1"=="" goto parse_done

if /I "%~1"=="--help" goto help

if /I "%~1"=="--debug" (
    if not "%BUILD_TYPE%"=="" (
        echo [error] Build type was already set to %BUILD_TYPE%.
        echo.
        goto help_error
    )
    set "BUILD_TYPE=Debug"
    shift
    goto parse_args
)

if /I "%~1"=="--release" (
    if not "%BUILD_TYPE%"=="" (
        echo [error] Build type was already set to %BUILD_TYPE%.
        echo.
        goto help_error
    )
    set "BUILD_TYPE=Release"
    shift
    goto parse_args
)

if /I "%~1"=="--reconfigure" (
    set "FORCE_CONFIGURE=1"
    shift
    goto parse_args
)

echo [error] Unknown flag: %~1
echo.
goto help_error

:parse_done
if "%BUILD_TYPE%"=="" (
    echo [status] No flag supplied. Defaulting to release.
    set "BUILD_TYPE=Release"
)

set "BUILD_DIR=%ROOT%\build\%BUILD_TYPE%"
set "CACHE_FILE=%BUILD_DIR%\CMakeCache.txt"

echo [status] Build type: %BUILD_TYPE%
echo [status] Build folder: %BUILD_DIR%
if "%FORCE_CONFIGURE%"=="1" echo [status] Configure: forced
echo.

echo [1/4] Checking tools...
where cmake >nul 2>nul
if errorlevel 1 (
    echo [error] CMake was not found in PATH.
    set "EXIT_CODE=1"
    goto done
)

where ninja >nul 2>nul
if errorlevel 1 (
    echo [error] Ninja was not found in PATH.
    set "EXIT_CODE=1"
    goto done
)

if exist "%ROOT%\.gitmodules" (
    where git >nul 2>nul
    if errorlevel 1 (
        echo [error] Git was not found in PATH.
        set "EXIT_CODE=1"
        goto done
    )
)

echo [2/4] Checking vendor dependencies...
if not exist "%ROOT%\vendor\glfw\CMakeLists.txt" set "VENDOR_WAS_MISSING=1"
if not exist "%ROOT%\vendor\glm\CMakeLists.txt" set "VENDOR_WAS_MISSING=1"
if not exist "%ROOT%\vendor\glad\cmake\CMakeLists.txt" set "VENDOR_WAS_MISSING=1"

if "%VENDOR_WAS_MISSING%"=="1" (
    if exist "%ROOT%\.gitmodules" (
        echo [status] Vendor files missing. Updating submodules...
        git -C "%ROOT%" submodule update --init --recursive --jobs 3 -- vendor/glfw vendor/glm vendor/glad
        if errorlevel 1 (
            echo [error] Vendor submodule update failed.
            set "EXIT_CODE=1"
            goto done
        )
    ) else (
        echo [status] No .gitmodules file found. Skipping submodule sync.
    )
) else (
    echo [status] Vendor dependencies already present. Skipping submodule sync.
)

call :check_vendor "GLFW" "%ROOT%\vendor\glfw\CMakeLists.txt"
call :check_vendor "GLM" "%ROOT%\vendor\glm\CMakeLists.txt"
call :check_vendor "glad" "%ROOT%\vendor\glad\cmake\CMakeLists.txt"

if "%MISSING_VENDOR%"=="1" (
    echo.
    echo [error] Vendor dependencies are missing and could not be fetched automatically.
    echo [error] If you cloned without submodules, run:
    echo [error]   git submodule update --init --recursive
    echo [error] If you are setting up the repo from scratch, add them once:
    echo [error]   git submodule add https://github.com/glfw/glfw.git vendor/glfw
    echo [error]   git submodule add https://github.com/g-truc/glm.git vendor/glm
    echo [error]   git submodule add -b glad2 https://github.com/Dav1dde/glad.git vendor/glad
    set "EXIT_CODE=1"
    goto done
)

if "%VENDOR_WAS_MISSING%"=="1" (
    echo [status] Vendor dependencies were initialized. Forcing configure.
    set "FORCE_CONFIGURE=1"
)

if "%FORCE_CONFIGURE%"=="1" (
    echo [3/4] Configuring...
    cmake -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
    if errorlevel 1 (
        echo [error] Configure failed.
        set "EXIT_CODE=1"
        goto done
    )
) else if exist "%CACHE_FILE%" (
    echo [3/4] Configure cache found. Skipping configure.
) else (
    echo [3/4] Configuring...
    cmake -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
    if errorlevel 1 (
        echo [error] Configure failed.
        set "EXIT_CODE=1"
        goto done
    )
)

echo.
echo [4/4] Building...
cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 (
    echo [error] Build failed.
    set "EXIT_CODE=1"
    goto done
)

echo.
echo [status] Build complete.
echo [status] Output: %BUILD_DIR%\lcdream.exe
goto done

:help
echo Usage:
echo   build.bat --debug
echo   build.bat --release
echo   build.bat --debug --reconfigure
echo   build.bat --release --reconfigure
echo   build.bat --help
echo.
echo No flag defaults to --release.
echo Use --reconfigure to regenerate CMake files before building.
goto done

:help_error
echo Usage:
echo   build.bat --debug
echo   build.bat --release
echo   build.bat --debug --reconfigure
echo   build.bat --release --reconfigure
echo   build.bat --help
echo.
set "EXIT_CODE=1"
goto done

:check_vendor
if not exist "%~2" (
    echo [error] Missing %~1 at %~2.
    set "MISSING_VENDOR=1"
)
exit /b 0

:done
echo.
echo Press any key to close...
pause >nul
exit /b %EXIT_CODE%
