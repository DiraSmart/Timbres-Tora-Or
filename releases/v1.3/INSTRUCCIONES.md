# Sistema de Timbres Tora Or - v1.3

## Novedades de la versión 1.3

- ✅ **Integración MQTT / Home Assistant** - Control remoto completo via MQTT
- ✅ **Auto-descubrimiento Home Assistant** - Los dispositivos aparecen automáticamente en HA
- ✅ **Switches individuales por timbre** - Control de cada timbre desde Home Assistant
- ✅ **Switch global del sistema** - Activar/desactivar todo el sistema remotamente
- ✅ **Sensores de estado** - Señal WiFi y dirección IP publicados a Home Assistant
- ✅ **Configuración MQTT en interfaz web** - Configuración completa desde la página de configuración
- ✅ **Backup/Restore incluye MQTT** - La configuración MQTT se guarda en los backups
- ✅ Actualización de GPIOs de relés para mejor compatibilidad (32, 33, 25, 26)
- ✅ Logo de la escuela en todas las páginas (login, principal y configuración)
- ✅ Logo "Powered by DiraSmart" al final de la página principal de horarios
- ✅ Mejoras en la interfaz de usuario

## Características de MQTT / Home Assistant

### Auto-descubrimiento Home Assistant

Cuando conectas el sistema a un broker MQTT y habilitas el auto-descubrimiento, Home Assistant detectará automáticamente:

**Switches (Interruptores):**
- `switch.timbre_1` - Control del Timbre 1
- `switch.timbre_2` - Control del Timbre 2
- `switch.timbre_3` - Control del Timbre 3
- `switch.timbre_4` - Control del Timbre 4
- `switch.sistema_timbres_global` - Switch global para activar/desactivar todo el sistema

**Sensores:**
- `sensor.timbres_wifi_signal` - Señal WiFi del ESP32 (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

### Topics MQTT

**Control de timbres:**
- `timbres/bell0/set` - Activar Timbre 1 (payload: `ON`)
- `timbres/bell1/set` - Activar Timbre 2 (payload: `ON`)
- `timbres/bell2/set` - Activar Timbre 3 (payload: `ON`)
- `timbres/bell3/set` - Activar Timbre 4 (payload: `ON`)
- `timbres/system/set` - Activar/Desactivar sistema global (payload: `ON`/`OFF`)

**Estado de timbres:**
- `timbres/bell0/state` - Estado del Timbre 1
- `timbres/bell1/state` - Estado del Timbre 2
- `timbres/bell2/state` - Estado del Timbre 3
- `timbres/bell3/state` - Estado del Timbre 4
- `timbres/system/state` - Estado del sistema global

**Eventos:**
- `timbres/bell0/event` - Eventos del Timbre 1 (`activated` / `deactivated`)
- `timbres/bell1/event` - Eventos del Timbre 2 (`activated` / `deactivated`)
- `timbres/bell2/event` - Eventos del Timbre 3 (`activated` / `deactivated`)
- `timbres/bell3/event` - Eventos del Timbre 4 (`activated` / `deactivated`)

**Sensores:**
- `timbres/wifi/rssi` - Señal WiFi (actualizado cada 30 segundos)
- `timbres/wifi/ip` - Dirección IP (actualizado cada 30 segundos)

## Cambios de hardware (IMPORTANTE)

### Nuevos GPIOs de relés:
- **Timbre 1**: GPIO 32 (IN1)
- **Timbre 2**: GPIO 33 (IN2)
- **Timbre 3**: GPIO 25 (IN3)
- **Timbre 4**: GPIO 26 (IN4)

Si estás actualizando desde v1.0, v1.1 o v1.2, deberás reconectar los relés a los nuevos pines.

## Archivos incluidos

- `tora-or-complete-v1.3.bin` - **Archivo unificado (RECOMENDADO)** - Incluye todo en un solo archivo
- `tora-or-firmware-v1.3.bin` - Firmware principal del sistema
- `tora-or-filesystem-v1.3.bin` - Sistema de archivos con páginas web y logo
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instrucciones de instalación

### Opción 1: ESPHome Web (MÁS FÁCIL) ⭐

1. Ve a https://web.esphome.io/
2. Conecta el ESP32 al puerto USB
3. Haz clic en "Connect"
4. Selecciona tu puerto COM
5. Haz clic en "Install"
6. Selecciona el archivo `tora-or-complete-v1.3.bin`
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
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x0 tora-or-complete-v1.3.bin
```

### Opción 3: Instalación por partes (avanzado)

Si prefieres instalar cada componente por separado:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 tora-or-firmware-v1.3.bin 0x290000 tora-or-filesystem-v1.3.bin
```

### Solo actualización de firmware y filesystem

Si solo quieres actualizar el firmware y filesystem sin tocar bootloader ni particiones:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x10000 tora-or-firmware-v1.3.bin 0x290000 tora-or-filesystem-v1.3.bin
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

## Configuración de MQTT / Home Assistant

### Paso 1: Configurar broker MQTT en Home Assistant

Si aún no tienes Mosquitto broker instalado:

1. En Home Assistant, ve a **Settings → Add-ons**
2. Busca e instala **Mosquitto broker**
3. Inicia el add-on y habilita "Start on boot"
4. Ve a **Configuration** y configura usuario y contraseña si lo deseas

### Paso 2: Configurar MQTT en el sistema de timbres

1. Accede a la interfaz web del sistema (http://IP_DEL_ESP32)
2. Inicia sesión como administrador (`dirasmart` / `dirasmart1`)
3. Ve a la página de **Configuración**
4. En la sección **Integración MQTT / Home Assistant**, configura:
   - **Habilitar MQTT**: ✅ Activar
   - **Servidor MQTT**: IP de tu Home Assistant (ej: `192.168.1.10`)
   - **Puerto**: `1883` (por defecto)
   - **Usuario MQTT**: Tu usuario MQTT (opcional)
   - **Contraseña MQTT**: Tu contraseña MQTT (opcional)
   - **Client ID**: `timbres-tora-or` (puedes dejarlo por defecto)
   - **Auto-descubrimiento HA**: ✅ Activar
5. Haz clic en **Guardar Configuración MQTT**

### Paso 3: Verificar en Home Assistant

1. Ve a **Settings → Devices & Services** en Home Assistant
2. Busca "MQTT" y abre la integración
3. Deberías ver un dispositivo llamado **"Sistema Timbres Tora Or"**
4. Haz clic en el dispositivo para ver todas las entidades disponibles

### Uso desde Home Assistant

Una vez configurado, puedes:

- Activar timbres manualmente desde la interfaz
- Crear automatizaciones basadas en horarios
- Recibir notificaciones cuando se activa un timbre
- Ver el historial de activaciones
- Controlar el sistema desde la aplicación móvil de Home Assistant

Para más información, consulta el archivo **INTEGRACION_HOME_ASSISTANT.md** en el repositorio.

## Actualización desde versión anterior

Si ya tienes una versión anterior instalada:

1. **Crear backup**: Ve a Configuración → Backup y Restauración → Descargar Backup
2. **Instalar v1.3**: Usa cualquiera de los métodos de instalación arriba
3. **Restaurar backup**: Ve a Configuración → Backup y Restauración → Restaurar Backup
   - El backup restaurará toda tu configuración excepto MQTT (que es nuevo)
4. **Configurar MQTT**: Si deseas usar MQTT, configúralo desde la página de configuración

## Soporte

Para más información o para reportar problemas:
- Documentación completa: https://github.com/DiraSmart/Timbres-Tora-Or
- Guía de integración con Home Assistant: INTEGRACION_HOME_ASSISTANT.md
- Reportar problemas: https://github.com/DiraSmart/Timbres-Tora-Or/issues
