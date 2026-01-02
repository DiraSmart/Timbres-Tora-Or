# Sistema de Timbres Tora Or - v1.2

## Novedades de la versión 1.2

- ✅ Actualización de GPIOs de relés para mejor compatibilidad (32, 33, 25, 26)
- ✅ Logo de la escuela en todas las páginas (login, principal y configuración)
- ✅ Logo "Powered by DiraSmart" al final de la página principal de horarios
- ✅ Manejador de archivos estáticos para servir imágenes PNG
- ✅ Corrección en nombres de archivos (sin espacios en URLs)
- ✅ FIX: Backup ahora guarda y restaura nombres de horarios correctamente
- ✅ Mejoras en la interfaz de usuario

## Cambios de hardware (IMPORTANTE)

### Nuevos GPIOs de relés:
- **Timbre 1**: GPIO 32 (IN1)
- **Timbre 2**: GPIO 33 (IN2)
- **Timbre 3**: GPIO 25 (IN3)
- **Timbre 4**: GPIO 26 (IN4)

Si estás actualizando desde v1.0 o v1.1, deberás reconectar los relés a los nuevos pines.

## Archivos incluidos

- `tora-or-complete-v1.2.bin` - **Archivo unificado (RECOMENDADO)** - Incluye todo en un solo archivo
- `tora-or-firmware-v1.2.bin` - Firmware principal del sistema
- `tora-or-filesystem-v1.2.bin` - Sistema de archivos con páginas web y logo
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instrucciones de instalación

### Opción 1: ESPHome Web (MÁS FÁCIL) ⭐

1. Ve a https://web.esphome.io/
2. Conecta el ESP32 al puerto USB
3. Haz clic en "Connect"
4. Selecciona tu puerto COM
5. Haz clic en "Install"
6. Selecciona el archivo `tora-or-complete-v1.2.bin`
7. Espera a que termine la instalación

### Opción 2: Línea de comandos con archivo unificado

#### Requisitos
- ESP32 Dev Module
- Cable USB
- Python con esptool instalado

#### Instalación

1. Conecta el ESP32 al puerto USB

2. Pon el ESP32 en modo de descarga:
   - Mantén presionado el botón BOOT
   - Conecta el USB (o presiona RESET si ya está conectado)
   - Mantén BOOT por 3-5 segundos más
   - Suelta BOOT

3. Ejecuta el siguiente comando (ajusta COM4 a tu puerto):

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x0 tora-or-complete-v1.2.bin
```

### Opción 3: Instalación por partes (avanzado)

Si prefieres instalar cada componente por separado:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 tora-or-firmware-v1.2.bin 0x290000 tora-or-filesystem-v1.2.bin
```

### Solo actualización de firmware y filesystem

Si solo quieres actualizar el firmware y filesystem sin tocar bootloader ni particiones:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x10000 tora-or-firmware-v1.2.bin 0x290000 tora-or-filesystem-v1.2.bin
```

## Conexiones hardware

### Módulo RTC DS3231 al ESP32

```
DS3231    →    ESP32
VCC       →    3.3V
GND       →    GND
SDA       →    GPIO 21 (SDA)
SCL       →    GPIO 22 (SCL)
```

### Módulo de 4 Relés al ESP32

```
Relé      →    ESP32
VCC       →    5V o VIN (según tu módulo)
GND       →    GND
IN1       →    GPIO 32 (Timbre 1)
IN2       →    GPIO 33 (Timbre 2)
IN3       →    GPIO 25 (Timbre 3)
IN4       →    GPIO 26 (Timbre 4)
```

## Primer acceso

1. El ESP32 creará una red WiFi llamada `Timbres-Escuela-Config`
2. Contraseña: `12345678`
3. Conéctate a esta red
4. Abre un navegador y ve a `http://192.168.4.1`
5. Serás redirigido a la configuración WiFi
6. Configura tu red WiFi
7. El sistema se conectará a tu red y podrás acceder desde su nueva IP

## Credenciales por defecto

- **Usuario administrador**: dirasmart
- **Contraseña**: dirasmart1

## Soporte

Para reportar problemas o sugerencias, visita:
https://github.com/DiraSmart/Timbres-Tora-Or/issues
