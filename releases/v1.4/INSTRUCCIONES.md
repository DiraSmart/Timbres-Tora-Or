# Sistema de Timbres Tora Or - v1.4

## Novedades de la versión 1.4

### Mejoras en MQTT / Home Assistant
- ✅ **Fix master schedule switch** - El switch maestro de horarios ahora actualiza correctamente todos los switches individuales cuando se cambia desde Home Assistant
- ✅ **Sincronización bidireccional mejorada** - Mejor sincronización de estados entre Home Assistant y la interfaz web del ESP32
- ✅ **Iconos uniformes** - Todos los switches de horarios usan icono de calendario (mdi:calendar-clock) y todos los botones de prueba usan icono de campana (mdi:bell-ring)
- ✅ **Nombre de dispositivo actualizado** - Cambio de "ESP32 Bell Controller" a "Bell Controller" en Home Assistant para mejor presentación

### Correcciones importantes
- ✅ **Fix error en restore** - Corregido el mensaje "archivo inválido" que aparecía al restaurar un backup exitosamente
- ✅ **Mejora en reconexión WiFi/MQTT** - La reconexión ahora ocurre DESPUÉS de enviar la respuesta HTTP, evitando que la respuesta se corte
- ✅ **Mejor manejo de errores** - Verificación de estado HTTP antes de parsear JSON en el frontend

### Feedback visual mejorado
- ✅ **Horarios desactivados visualmente** - Los horarios se muestran en gris cuando están desactivados desde el switch maestro
- ✅ **Mejor indicación de estado** - Mensaje claro cuando los horarios programados están desactivados

## Integración MQTT / Home Assistant

Esta versión mantiene todas las características MQTT de v1.3 con las mejoras mencionadas:

### Entidades disponibles en Home Assistant

**Botones (para activación manual):**
- `button.timbre_1` - Activar Timbre 1 manualmente
- `button.timbre_2` - Activar Timbre 2 manualmente
- `button.timbre_3` - Activar Timbre 3 manualmente
- `button.timbre_4` - Activar Timbre 4 manualmente

**Switches de horarios programados:**
- `switch.horarios_timbre_1` - Activar/desactivar horarios del Timbre 1
- `switch.horarios_timbre_2` - Activar/desactivar horarios del Timbre 2
- `switch.horarios_timbre_3` - Activar/desactivar horarios del Timbre 3
- `switch.horarios_timbre_4` - Activar/desactivar horarios del Timbre 4
- `switch.horarios_programados` - Switch maestro que activa/desactiva TODOS los horarios

**Sensores:**
- `sensor.timbres_wifi_signal` - Señal WiFi del ESP32 (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

## Archivos incluidos

- `tora-or-firmware-v1.4.bin` - Firmware principal del sistema
- `tora-or-filesystem-v1.4.bin` - Sistema de archivos con páginas web
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instrucciones de instalación

### Opción 1: Actualización OTA desde la interfaz web (MÁS FÁCIL) ⭐

Si ya tienes una versión anterior instalada:

1. Accede a la interfaz web del sistema (http://IP_DEL_ESP32/ota)
2. La página de actualización OTA está disponible sin autenticación
3. Selecciona `tora-or-firmware-v1.4.bin`
4. Haz clic en "Actualizar"
5. Espera a que termine (el ESP32 se reiniciará automáticamente)

**Nota:** La actualización OTA solo actualiza el firmware, no el filesystem. Si hay cambios importantes en las páginas web, se recomienda usar esptool.

### Opción 2: ESPHome Web (INSTALACIÓN LIMPIA)

1. Ve a https://web.esphome.io/
2. Conecta el ESP32 al puerto USB
3. Haz clic en "Connect"
4. Selecciona tu puerto COM
5. Haz clic en "Install" → "Manual installation"
6. Sube los archivos en este orden:
   - `0x1000`: `bootloader.bin`
   - `0x8000`: `partitions.bin`
   - `0x10000`: `tora-or-firmware-v1.4.bin`
   - `0x290000`: `tora-or-filesystem-v1.4.bin`

### Opción 3: Línea de comandos (instalación completa)

#### Requisitos
- ESP32 Dev Module
- Cable USB
- Python con esptool instalado

#### Instalación completa

1. Conecta el ESP32 al puerto USB

2. Pon el ESP32 en modo de descarga:
   - Mantén presionado el botón BOOT
   - Conecta el USB (o presiona RESET si ya está conectado)
   - Mantén BOOT por 3-5 segundos más
   - Suelta BOOT

3. Ejecuta el siguiente comando (ajusta COM4 a tu puerto):

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 tora-or-firmware-v1.4.bin 0x290000 tora-or-filesystem-v1.4.bin
```

### Opción 4: Solo actualización de firmware y filesystem

Si solo quieres actualizar el firmware y filesystem sin tocar bootloader ni particiones:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x10000 tora-or-firmware-v1.4.bin 0x290000 tora-or-filesystem-v1.4.bin
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

### Nuevo en v1.4: Republicar Discovery

Si los switches no aparecen correctamente en Home Assistant:

1. En la página de configuración del sistema (http://IP_DEL_ESP32/config.html)
2. Baja hasta la sección "Integración MQTT / Home Assistant"
3. Click en el botón **"🔄 Republicar Discovery"**
4. Espera 10 segundos
5. Verifica en Home Assistant que los dispositivos se hayan actualizado

## Actualización desde versión anterior

### Desde v1.3 o anterior:

1. **Crear backup**: Ve a Configuración → Backup y Restauración → Descargar Backup
2. **Instalar v1.4**: Usa el método OTA o cualquiera de los métodos de instalación arriba
3. **Restaurar backup**: Ve a Configuración → Backup y Restauración → Restaurar Backup
   - El backup restaurará toda tu configuración incluyendo MQTT
   - **Nuevo en v1.4**: Ya no verás el error "archivo inválido" al restaurar

### ¿Qué se conserva al actualizar?

- ✅ Nombres de timbres personalizados
- ✅ Duraciones de timbres
- ✅ Horarios programados
- ✅ Configuración WiFi
- ✅ Configuración MQTT
- ✅ Usuario normal (si está configurado)
- ✅ Visibilidad de timbres

## Soporte

Para más información o para reportar problemas:
- Documentación completa: https://github.com/DiraSmart/Timbres-Tora-Or
- Guía de integración con Home Assistant: INTEGRACION_HOME_ASSISTANT.md
- Reportar problemas: https://github.com/DiraSmart/Timbres-Tora-Or/issues
