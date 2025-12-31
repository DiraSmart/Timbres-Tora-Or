# Tora Or - Sistema de Timbres v1.0

## Archivos incluidos

- `tora-or-firmware-v1.0.bin` - Firmware principal del ESP32
- `tora-or-filesystem-v1.0.bin` - Sistema de archivos con las páginas web
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instalación Completa (Primera vez)

### Opción 1: Usando PlatformIO (Recomendado)

```bash
# Clonar el repositorio
git clone <url-repositorio>
cd "Proyecto Tora Ohr"

# Subir firmware
pio run --target upload

# Subir filesystem
pio run --target uploadfs
```

### Opción 2: Usando esptool (Manual)

**Importante:** Conecta el ESP32 y presiona el botón BOOT al momento de subir.

```bash
# Subir todo en una sola operación (desde la carpeta releases/v1.0)
esptool.py --chip esp32 --port COM4 --baud 921600 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 tora-or-firmware-v1.0.bin \
  0x290000 tora-or-filesystem-v1.0.bin
```

**Nota:** Reemplaza `COM4` con tu puerto serial correspondiente.

## Actualización OTA (Firmware únicamente)

Si ya tienes una versión anterior instalada, puedes actualizar solo el firmware sin cable:

1. Accede a la interfaz web del ESP32
2. Login como administrador (usuario: `dirasmart`, contraseña: `dirasmart1`)
3. Ve a Configuración → Actualización de Firmware
4. Selecciona el archivo `tora-or-firmware-v1.0.bin`
5. El ESP32 se reiniciará automáticamente con la nueva versión

**Importante:** La actualización OTA NO actualiza el filesystem (las páginas web). Para actualizar el filesystem debes usar el método manual con cable.

## Credenciales por defecto

- **Usuario Admin:** `dirasmart`
- **Contraseña Admin:** `dirasmart1`

## Características de esta versión

- ✅ Control de 4 timbres independientes
- ✅ Programación de horarios con selección de días
- ✅ Sistema de autenticación (Admin + Usuario normal)
- ✅ Nombres personalizables para timbres y horarios
- ✅ Control de visibilidad de timbres
- ✅ Backup/Restore de configuración en JSON
- ✅ Actualización OTA de firmware
- ✅ Switch global para activar/desactivar todos los horarios
- ✅ Duración configurable por timbre (1-60 segundos)
- ✅ Captive Portal WiFi para configuración inicial
- ✅ Modo Access Point automático si pierde WiFi
- ✅ Reconexión automática cada 2 minutos
- ✅ Sincronización NTP automática
- ✅ RTC DS3231 para funcionamiento sin internet

## Requisitos de Hardware

- ESP32 DevKit (cualquier modelo)
- Módulo RTC DS3231 con batería CR2032
- Módulo de 4 Relés (5V o 3.3V)
- 4 Timbres o dispositivos a controlar
- Fuente de alimentación adecuada

## Soporte

Para problemas o preguntas, consulta el README.md principal del repositorio.
