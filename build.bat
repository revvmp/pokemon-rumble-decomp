@echo off
setlocal
cd /d "%~dp0"

where python >nul 2>nul
if errorlevel 1 (
    echo Python was not found. Install Python 3.10 or newer from https://www.python.org/downloads/ and tick "Add python.exe to PATH".
    exit /b 1
)
python -c "import sys; sys.exit(sys.version_info < (3, 10))"
if errorlevel 1 (
    echo Python 3.10 or newer is required.
    exit /b 1
)

echo Checking the local original
python tools\setup\check_reference.py
if errorlevel 1 goto failed

echo [1/5] Downloading and checking pinned tools
python tools\setup\acquire_tools.py
if errorlevel 1 goto failed

echo [2/5] Installing ninja
if not exist .venv\Scripts\ninja.exe (
    python -m venv .venv
    if errorlevel 1 goto failed
    .venv\Scripts\python -m pip install --disable-pip-version-check --quiet ninja==1.13.0
    if errorlevel 1 goto failed
)

echo [3/5] Configuring
python configure.py --no-progress
if errorlevel 1 goto failed

echo [4/5] Building
if exist build\wpse-v1\main.dol del /q build\wpse-v1\main.dol
if exist build\wpse-v1\main.dol goto failed
.venv\Scripts\ninja build/wpse-v1/main.dol
if errorlevel 1 goto failed

echo [5/5] Verifying main.dol against the original
python tools\verify\verify.py verify --manifest config\targets\wpse-v1.json --reference-dir orig\wpse-v1 --build-dir build\wpse-v1 --report build\wpse-v1\verification.json
if errorlevel 1 goto failed

echo.
echo Build succeeded: build\wpse-v1\main.dol matches the original.
exit /b 0

:failed
echo.
echo Build failed. See the messages above for details.
exit /b 1
