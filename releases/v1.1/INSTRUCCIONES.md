# Sistema de Timbres Tora Or - v1.1

## Novedades de la versión 1.1

- ✅ Logo de la escuela en todas las páginas (login, principal y configuración)
- ✅ Manejador de archivos estáticos para servir imágenes PNG
- ✅ Corrección en nombres de archivos (sin espacios en URLs)
- ✅ Mejoras en la interfaz de usuario

## Archivos incluidos

- `tora-or-firmware-v1.1.bin` - Firmware principal del sistema
- `tora-or-filesystem-v1.1.bin` - Sistema de archivos con páginas web y logo
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instrucciones de instalación

### Requisitos
- ESP32 Dev Module
- Cable USB
- Python con esptool instalado

### Instalación completa (primera vez)

1. Conecta el ESP32 al puerto USB

2. Pon el ESP32 en modo de descarga:
   - Mantén presionado el botón BOOT
   - Conecta el USB (o presiona RESET si ya está conectado)
   - Mantén BOOT por 3-5 segundos más
   - Suelta BOOT

3. Ejecuta el siguiente comando (ajusta COM4 a tu puerto):

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 tora-or-firmware-v1.1.bin 0x290000 tora-or-filesystem-v1.1.bin
```

### Actualización (si ya tienes una versión anterior)

Solo necesitas actualizar el firmware y filesystem:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x10000 tora-or-firmware-v1.1.bin 0x290000 tora-or-filesystem-v1.1.bin
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
