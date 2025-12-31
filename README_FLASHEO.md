# Guía de Flasheo - Sistema de Timbres Tora Or

Esta guía te ayudará a cargar el firmware en tu ESP32 de forma fácil.

## 🚀 Método Rápido (Recomendado)

### Paso 1: Compilar Binarios

**⚠️ REQUISITO:** Debes tener PlatformIO instalado. Si no lo tienes, lee primero: [INSTALACION_PLATFORMIO.md](INSTALACION_PLATFORMIO.md)

Ejecuta el archivo `build_binaries.bat` haciendo doble click. Esto generará una carpeta `binarios/` con todos los archivos necesarios.

O desde terminal:
```bash
build_binaries.bat
```

### Paso 2: Flashear con ESPHome Web

1. **Abre el flasheador web**: https://web.esphome.io/

2. **Conecta el ESP32**:
   - Conecta tu ESP32 via USB
   - Click en "Connect"
   - Selecciona el puerto COM

3. **Carga los archivos** en este orden:

   | Dirección | Archivo | Descripción |
   |-----------|---------|-------------|
   | `0x1000` | `bootloader.bin` | Bootloader |
   | `0x8000` | `partitions.bin` | Particiones |
   | `0xE000` | `boot_app0.bin` | Boot App |
   | `0x10000` | `firmware.bin` | **Tu código principal** |
   | `0x310000` | `littlefs.bin` | **Archivos web (HTML)** |

4. **Instalar**: Click en "Install" y espera

5. **¡Listo!** El ESP32 se reiniciará automáticamente

## 📁 Archivos Generados

Después de ejecutar `build_binaries.bat`, encontrarás:

```
binarios/
├── bootloader.bin    (Bootloader del ESP32)
├── partitions.bin    (Tabla de particiones)
├── boot_app0.bin     (Inicializador)
├── firmware.bin      (Tu programa - el más importante)
└── littlefs.bin      (Tus archivos HTML/CSS/JS)
```

## 🔧 Métodos Alternativos

### Con PlatformIO (si lo tienes instalado)

```bash
# Flashear todo automáticamente
pio run --target upload      # Sube el código
pio run --target uploadfs     # Sube archivos HTML
```

### Con esptool.py (línea de comandos)

```bash
esptool.py --chip esp32 --port COM3 --baud 921600 ^
    write_flash -z ^
    0x1000 binarios/bootloader.bin ^
    0x8000 binarios/partitions.bin ^
    0xe000 binarios/boot_app0.bin ^
    0x10000 binarios/firmware.bin ^
    0x310000 binarios/littlefs.bin
```

(Reemplaza `COM3` con tu puerto)

## 🆘 Solución de Problemas

### "Failed to connect"
1. Mantén presionado el botón **BOOT** del ESP32
2. Click en "Connect"
3. Suelta **BOOT** cuando comience a flashear

### "Port not found"
- Instala drivers USB:
  - CH340: https://www.wch.cn/downloads/CH341SER_ZIP.html
  - CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

### No compila con `build_binaries.bat`
- Verifica que PlatformIO esté instalado:
  ```bash
  pio --version
  ```
- Si no está instalado, sigue la guía: [INSTALACION_PLATFORMIO.md](INSTALACION_PLATFORMIO.md)

### ESP32 no arranca después de flashear
1. Presiona el botón **RESET** del ESP32
2. Abre el Monitor Serial (115200 baud)
3. Deberías ver mensajes del sistema

## ✅ Verificación Post-Flasheo

1. **Abre el Monitor Serial** (115200 baud)
2. Deberías ver:
   ```
   === Sistema de Timbres Tora Or ===
   Modo: Access Point
   Conéctate a: Timbres-Escuela-Config
   IP del AP: 192.168.4.1
   ```

3. **Conéctate al WiFi**:
   - Red: `Timbres-Escuela-Config`
   - Contraseña: `12345678`

4. **Abre el navegador**:
   - Portal WiFi: `http://192.168.4.1/wifi_config.html`
   - Panel Principal: `http://192.168.4.1`

## 🔄 Actualizar Solo el Código

Si modificas el código C++ pero NO los archivos HTML:

**Con ESPHome Web:**
- Solo flashea `firmware.bin` en la dirección `0x10000`

**Con PlatformIO:**
```bash
pio run --target upload
```

Esto mantiene:
- ✅ Configuración WiFi
- ✅ Nombres de timbres
- ✅ Horarios programados

## 🎨 Actualizar Solo Archivos Web

Si modificas HTML/CSS/JS pero NO el código C++:

**Con ESPHome Web:**
- Solo flashea `littlefs.bin` en la dirección `0x310000`

**Con PlatformIO:**
```bash
pio run --target uploadfs
```

⚠️ **ADVERTENCIA**: Esto borrará:
- ❌ Configuración WiFi guardada
- ❌ Nombres de timbres
- ❌ Horarios programados

## 🔥 Reseteo Completo

Para borrar completamente el ESP32:

```bash
esptool.py --chip esp32 --port COM3 erase_flash
```

Luego flashea todos los archivos de nuevo.

## 📊 Información Técnica

| Dirección | Tamaño | Contenido |
|-----------|--------|-----------|
| 0x1000 | 4 KB | Bootloader |
| 0x8000 | 4 KB | Partition Table |
| 0xE000 | 4 KB | Boot App0 |
| 0x10000 | ~3 MB | Firmware (tu código) |
| 0x310000 | 64 KB | LittleFS (archivos web) |

Tamaño real del proyecto:
- Firmware: ~800 KB
- Sistema de archivos: ~20 KB

## 🌐 Enlaces Útiles

- **ESPHome Web Flasher**: https://web.esphome.io/
- **PlatformIO**: https://platformio.org/
- **Drivers CH340**: https://www.wch.cn/downloads/CH341SER_ZIP.html
- **Drivers CP2102**: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

## ❓ ¿Necesitas Más Ayuda?

- 📖 Lee el `README.md` principal
- 🔌 Revisa `DIAGRAMA_CONEXIONES.txt` para el hardware
- 📝 Consulta `flash_instructions.txt` para detalles técnicos
- 💬 Revisa el monitor serial para mensajes de error
