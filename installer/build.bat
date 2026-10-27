@echo off
set "ISCC_EXE="

if exist "C:\Users\%USERNAME%\AppData\Local\Programs\Inno Setup 6\ISCC.exe" (
    set "ISCC_EXE=C:\Users\%USERNAME%\AppData\Local\Programs\Inno Setup 6\ISCC.exe"
    goto :found
)
if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" (
    set "ISCC_EXE=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
    goto :found
)
if exist "C:\Program Files\Inno Setup 6\ISCC.exe" (
    set "ISCC_EXE=C:\Program Files\Inno Setup 6\ISCC.exe"
    goto :found
)
for %%I in (iscc.exe) do set "ISCC_EXE=%%~$PATH:I"
if defined ISCC_EXE goto :found

echo [ERROR] Inno Setup compiler (ISCC.exe) not found.
exit /b 1

:found
echo [LibreOMR] Compiling installer with %ISCC_EXE%...
"%ISCC_EXE%" "%~dp0libreomr.iss"
if %errorlevel% neq 0 (
    echo [ERROR] Inno Setup compilation failed with code %errorlevel%.
    exit /b %errorlevel%
)
echo [LibreOMR] Installer generated successfully in dist\
