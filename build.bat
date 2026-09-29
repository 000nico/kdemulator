@echo off
REM Build de kdemulator con MinGW-w64 UCRT64 (MSYS2).
REM El PATH se ajusta aqui para que funcione desde cmd/PowerShell/Zed
REM sin tener que abrir la terminal de MSYS2.

set "MINGW_BIN=C:\msys64\ucrt64\bin"
if not exist "%MINGW_BIN%\g++.exe" (
  echo [error] No se encontro g++.exe en "%MINGW_BIN%".
  echo         Instala MSYS2 y el paquete mingw-w64-ucrt-x86_64-gcc.
  exit /b 1
)

set "PATH=%MINGW_BIN%;%PATH%"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug || exit /b 1
copy /y build\compile_commands.json compile_commands.json >nul 2>&1
cmake --build build -j8 || exit /b 1

echo.
echo Build ok: build\kdemulator.exe
