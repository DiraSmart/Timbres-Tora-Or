@echo off
chcp 65001 >nul
echo ========================================
echo  Compilando Sistema de Timbres Tora Or
echo ========================================
echo.

REM Verificar si PlatformIO esta instalado
where pio >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: PlatformIO no esta instalado o no esta en el PATH
    echo.
    echo Por favor instala PlatformIO de una de estas formas:
    echo.
    echo 1. PlatformIO CLI:
    echo    https://platformio.org/install/cli
    echo.
    echo 2. PlatformIO IDE VSCode extension:
    echo    https://platformio.org/install/ide?install=vscode
    echo.
    echo 3. O usa Arduino IDE para compilar el proyecto
    echo.
    pause
    exit /b 1
)

REM Compilar el proyecto
echo [1/3] Compilando firmware...
call pio run

if %errorlevel% neq 0 (
    echo.
    echo ERROR: La compilacion fallo
    pause
    exit /b 1
)

echo.
echo [2/3] Compilando sistema de archivos LittleFS...
call pio run --target buildfs

if %errorlevel% neq 0 (
    echo.
    echo ERROR: La compilacion del sistema de archivos fallo
    pause
    exit /b 1
)

echo.
echo [3/3] Copiando archivos binarios...

REM Crear carpeta de salida
if not exist "binarios" mkdir binarios

REM Copiar firmware
copy ".pio\build\esp32dev\firmware.bin" "binarios\firmware.bin" >nul
if %errorlevel% neq 0 (
    echo ERROR: No se encontro firmware.bin
    pause
    exit /b 1
)

REM Copiar bootloader
copy "%USERPROFILE%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\bin\bootloader_dio_40m.bin" "binarios\bootloader.bin" >nul
if %errorlevel% neq 0 (
    echo ADVERTENCIA: No se encontro bootloader.bin
)

REM Copiar particiones
copy ".pio\build\esp32dev\partitions.bin" "binarios\partitions.bin" >nul
if %errorlevel% neq 0 (
    echo ADVERTENCIA: No se encontro partitions.bin
)

REM Copiar sistema de archivos
copy ".pio\build\esp32dev\littlefs.bin" "binarios\littlefs.bin" >nul
if %errorlevel% neq 0 (
    echo ERROR: No se encontro littlefs.bin
    pause
    exit /b 1
)

REM Copiar boot_app0
copy "%USERPROFILE%\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin" "binarios\boot_app0.bin" >nul
if %errorlevel% neq 0 (
    echo ADVERTENCIA: No se encontro boot_app0.bin
)

echo.
echo ========================================
echo  COMPILACION EXITOSA
echo ========================================
echo.
echo Archivos generados en la carpeta binarios:
echo.
dir /B binarios
echo.
echo Direcciones de memoria para flasheo:
echo   0x1000  - bootloader.bin
echo   0x8000  - partitions.bin
echo   0xE000  - boot_app0.bin
echo   0x10000 - firmware.bin
echo   0x310000 - littlefs.bin
echo.
echo Para flashear via web:
echo 1. Ve a: https://web.esphome.io/
echo 2. Conecta el ESP32 via USB
echo 3. Click en Connect
echo 4. Click en Install y sube cada .bin a su direccion
echo.
echo O usa el archivo flash_instructions.txt para mas detalles
echo.
pause
