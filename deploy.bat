@echo off
REM Growtopia Server Deployment Script for Windows
REM This script pulls the latest code from GitHub and rebuilds the server

echo 🚀 Starting Growtopia Server deployment...

REM Check if git is installed
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed. Please install git first.
    pause
    exit /b 1
)

REM Check if we're in a git repository
if not exist ".git" (
    echo [ERROR] This is not a git repository. Please run this script from the project root.
    pause
    exit /b 1
)

REM Backup current config if it exists
if exist "config.ini" (
    echo [INFO] Backing up current config.ini...
    copy config.ini config.ini.backup >nul
)

REM Pull latest changes from GitHub
echo [INFO] Pulling latest changes from GitHub...
git fetch origin

REM Check if there are any changes (simplified for Windows)
echo [INFO] Checking for updates...
git status --porcelain >nul
if errorlevel 1 (
    echo [WARNING] Local changes detected. Consider stashing them.
)

REM Pull the changes
git pull origin main
if errorlevel 1 (
    echo [ERROR] Failed to pull changes. Please check for conflicts.
    pause
    exit /b 1
)

echo [INFO] Successfully pulled latest changes.

REM Restore config backup if it was backed up
if exist "config.ini.backup" (
    echo [INFO] Restoring config.ini backup...
    move config.ini.backup config.ini >nul
)

REM Build the project
echo [INFO] Building the project...
if exist "build_windows.bat" (
    call build_windows.bat
    if errorlevel 1 (
        echo [ERROR] Build failed. Please check the build output.
        pause
        exit /b 1
    )
    echo [INFO] Build successful!
) else (
    echo [WARNING] No build script found. Skipping build step.
)

echo [INFO] ✅ Deployment completed successfully!
echo [INFO] 📝 Don't forget to:
echo [INFO]    - Check your config.ini settings
echo [INFO]    - Review any local changes
echo [INFO]    - Test your server before deploying to production

echo.
echo 🔧 Useful commands:
echo    - View git status: git status
echo    - View recent commits: git log --oneline -10
echo    - Check differences: git diff

pause