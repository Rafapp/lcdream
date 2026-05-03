@echo off
setlocal

title LCDream Build

for %%I in ("%~dp0.") do set "ROOT=%%~fI"
set "EXIT_CODE=0"
set "BUILD_TYPE="

echo ====================
echo Building LCDream ... 
echo ====================
echo.

if /I "%~1"=="--help" goto help
if /I "%~1"=="--debug" set "BUILD_TYPE=Debug"
if /I "%~1"=="--release" set "BUILD_TYPE=Release"

if not "%~2"=="" (
    echo [error] Too many arguments.
    echo.
    goto help_error
)

if "%~1"=="" (
    echo [status] No flag supplied. Defaulting to release.
    set "BUILD_TYPE=Release"
)

if "%BUILD_TYPE%"=="" (
    echo [error] Unknown flag: %~1
    echo.
    goto help_error
)

set "BUILD_DIR=%ROOT%\build\%BUILD_TYPE%"

echo [status] Build type: %BUILD_TYPE%
echo [status] Build folder: %BUILD_DIR%
echo.

echo [1/3] Checking tools...
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

echo [2/3] Configuring...
cmake -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo [error] Configure failed.
    set "EXIT_CODE=1"
    goto done
)

echo.
echo [3/3] Building...
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
echo   build.bat --help
echo.
echo No flag defaults to --release.
goto done

:help_error
echo Usage:
echo   build.bat --debug
echo   build.bat --release
echo   build.bat --help
echo.
set "EXIT_CODE=1"
goto done

:done
echo.
echo Press any key to close...
pause >nul
exit /b %EXIT_CODE%
