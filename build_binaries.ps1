# Script PowerShell para compilar binarios del Sistema de Timbres Tora Or
# =========================================================================

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " Compilando Sistema de Timbres Tora Or" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar si PlatformIO está instalado
$pioExists = Get-Command pio -ErrorAction SilentlyContinue
if (-not $pioExists) {
    Write-Host "ERROR: PlatformIO no esta instalado o no esta en el PATH" -ForegroundColor Red
    Write-Host ""
    Write-Host "Por favor instala PlatformIO de una de estas formas:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "1. PlatformIO CLI:"
    Write-Host "   https://platformio.org/install/cli"
    Write-Host ""
    Write-Host "2. PlatformIO IDE (VSCode extension):"
    Write-Host "   https://platformio.org/install/ide?install=vscode"
    Write-Host ""
    Write-Host "3. O usa Arduino IDE para compilar el proyecto"
    Write-Host ""
    Read-Host "Presiona Enter para salir"
    exit 1
}

# Compilar el proyecto
Write-Host "[1/3] Compilando firmware..." -ForegroundColor Yellow
& pio run
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: La compilacion fallo" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit 1
}

Write-Host ""
Write-Host "[2/3] Compilando sistema de archivos LittleFS..." -ForegroundColor Yellow
& pio run --target buildfs
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: La compilacion del sistema de archivos fallo" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit 1
}

Write-Host ""
Write-Host "[3/3] Copiando archivos binarios..." -ForegroundColor Yellow

# Crear carpeta de salida
if (-not (Test-Path "binarios")) {
    New-Item -ItemType Directory -Path "binarios" | Out-Null
}

# Copiar firmware
$firmwarePath = ".pio\build\esp32dev\firmware.bin"
if (Test-Path $firmwarePath) {
    Copy-Item $firmwarePath "binarios\firmware.bin" -Force
    Write-Host "  ✓ firmware.bin copiado" -ForegroundColor Green
} else {
    Write-Host "  ✗ ERROR: No se encontro firmware.bin" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit 1
}

# Copiar bootloader
$bootloaderPath = "$env:USERPROFILE\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\bin\bootloader_dio_40m.bin"
if (Test-Path $bootloaderPath) {
    Copy-Item $bootloaderPath "binarios\bootloader.bin" -Force
    Write-Host "  ✓ bootloader.bin copiado" -ForegroundColor Green
} else {
    Write-Host "  ⚠ ADVERTENCIA: No se encontro bootloader.bin" -ForegroundColor Yellow
}

# Copiar particiones
$partitionsPath = ".pio\build\esp32dev\partitions.bin"
if (Test-Path $partitionsPath) {
    Copy-Item $partitionsPath "binarios\partitions.bin" -Force
    Write-Host "  ✓ partitions.bin copiado" -ForegroundColor Green
} else {
    Write-Host "  ⚠ ADVERTENCIA: No se encontro partitions.bin" -ForegroundColor Yellow
}

# Copiar sistema de archivos
$littlefsPath = ".pio\build\esp32dev\littlefs.bin"
if (Test-Path $littlefsPath) {
    Copy-Item $littlefsPath "binarios\littlefs.bin" -Force
    Write-Host "  ✓ littlefs.bin copiado" -ForegroundColor Green
} else {
    Write-Host "  ✗ ERROR: No se encontro littlefs.bin" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit 1
}

# Copiar boot_app0
$bootApp0Path = "$env:USERPROFILE\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin"
if (Test-Path $bootApp0Path) {
    Copy-Item $bootApp0Path "binarios\boot_app0.bin" -Force
    Write-Host "  ✓ boot_app0.bin copiado" -ForegroundColor Green
} else {
    Write-Host "  ⚠ ADVERTENCIA: No se encontro boot_app0.bin" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host " COMPILACION EXITOSA" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Archivos generados en la carpeta 'binarios':" -ForegroundColor Cyan
Write-Host ""
Get-ChildItem "binarios" | ForEach-Object { Write-Host "  - $($_.Name)" -ForegroundColor White }
Write-Host ""
Write-Host "Direcciones de memoria para flasheo:" -ForegroundColor Yellow
Write-Host "  0x1000   - bootloader.bin"
Write-Host "  0x8000   - partitions.bin"
Write-Host "  0xE000   - boot_app0.bin"
Write-Host "  0x10000  - firmware.bin"
Write-Host "  0x310000 - littlefs.bin"
Write-Host ""
Write-Host "Para flashear via web:" -ForegroundColor Cyan
Write-Host "1. Ve a: https://web.esphome.io/"
Write-Host "2. Conecta el ESP32 via USB"
Write-Host "3. Click en 'Connect'"
Write-Host "4. Click en 'Install' y sube cada .bin a su direccion"
Write-Host ""
Write-Host "O usa el archivo 'flash_instructions.txt' para mas detalles" -ForegroundColor Gray
Write-Host ""
Read-Host "Presiona Enter para salir"
