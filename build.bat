@echo off
setlocal

title LCDream Build

for %%I in ("%~dp0.") do set "ROOT=%%~fI"
set "EXIT_CODE=0"
set "BUILD_TYPE="
set "FORCE_CONFIGURE=0"

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

if "%FORCE_CONFIGURE%"=="1" (
    echo [2/3] Configuring...
    cmake -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
    if errorlevel 1 (
        echo [error] Configure failed.
        set "EXIT_CODE=1"
        goto done
    )
) else if exist "%CACHE_FILE%" (
    echo [2/3] Configure cache found. Skipping configure.
) else (
    echo [2/3] Configuring...
    cmake -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
    if errorlevel 1 (
        echo [error] Configure failed.
        set "EXIT_CODE=1"
        goto done
    )
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

:done
echo.
echo Press any key to close...
pause >nul
exit /b %EXIT_CODE%
