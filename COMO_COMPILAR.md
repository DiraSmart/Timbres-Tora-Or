# Cómo Compilar el Proyecto

Hay diferentes formas de compilar el proyecto dependiendo de qué terminal uses.

## Opción 1: PowerShell (Recomendado para Windows 10/11)

Si estás usando PowerShell (como se ve en tu terminal con `PS C:\...`):

```powershell
.\build_binaries.ps1
```

**Si obtienes error de permisos**, ejecuta primero:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Luego intenta de nuevo:
```powershell
.\build_binaries.ps1
```

---

## Opción 2: CMD (Símbolo del Sistema)

Si usas CMD tradicional:

```cmd
build_binaries.bat
```

O simplemente haz doble-click en el archivo `build_binaries.bat`.

---

## Opción 3: Desde PowerShell ejecutar el .bat

Si estás en PowerShell pero quieres usar el .bat:

```powershell
cmd /c build_binaries.bat
```

---

## Opción 4: Directamente con PlatformIO

Si prefieres ejecutar los comandos manualmente:

```bash
# Compilar firmware
pio run

# Compilar archivos web (LittleFS)
pio run --target buildfs
```

Luego copia manualmente los archivos desde `.pio\build\esp32dev\` a la carpeta `binarios\`.

---

## Opción 5: VSCode con PlatformIO

Si tienes VSCode con PlatformIO instalado:

1. Abre la carpeta del proyecto en VSCode
2. Click en el ícono de PlatformIO (hormiga) en la barra lateral
3. En **PROJECT TASKS**:
   - Click en `esp32dev → Build` (compila firmware)
   - Click en `esp32dev → Build Filesystem Image` (compila archivos web)
4. Los archivos compilados estarán en `.pio\build\esp32dev\`

---

## ¿Qué Script Usar?

| Terminal | Comando |
|----------|---------|
| **PowerShell** (azul con `PS>`) | `.\build_binaries.ps1` |
| **CMD** (negro con `C:\>`) | `build_binaries.bat` |
| **Git Bash / WSL** | `cmd.exe /c build_binaries.bat` |

---

## Archivos que se Generan

Después de compilar, en la carpeta `binarios\` encontrarás:

- `firmware.bin` - Programa principal
- `littlefs.bin` - Archivos web (HTML, CSS, JS)
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones
- `boot_app0.bin` - Configuración de arranque

---

## Solución de Problemas

### "pio no se reconoce como comando"
- PlatformIO no está instalado
- Lee: [INSTALACION_PLATFORMIO.md](INSTALACION_PLATFORMIO.md)

### "was unexpected at this time"
- Estás en PowerShell pero intentando ejecutar el .bat
- Usa `.\build_binaries.ps1` en su lugar
- O ejecuta: `cmd /c build_binaries.bat`

### Error de permisos en PowerShell
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### "No se encontró firmware.bin"
- La compilación falló antes
- Revisa los errores de compilación
- Verifica que tengas todas las librerías instaladas

---

## Siguiente Paso

Una vez compilado exitosamente, sigue la guía de flasheo:
- [README_FLASHEO.md](README_FLASHEO.md)
