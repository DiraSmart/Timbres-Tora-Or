# Sistema de Timbres Tora Or - v1.6

## Novedades de la versión 1.6

### 🔒 Correcciones críticas de seguridad

- ✅ **OTA requiere autenticación admin** - El endpoint de actualización OTA ahora verifica autenticación de administrador antes de permitir actualizaciones
- ✅ **Backup restringido a admin** - La descarga de backup ahora requiere autenticación de administrador para proteger credenciales WiFi y MQTT
- ✅ **Tokens criptográficamente seguros** - Cambio de `random()` a `esp_random()` para generación de tokens de sesión más seguros

### 🧹 Limpieza de código

- ✅ **Código obsoleto eliminado** - Eliminadas variables y suscripciones MQTT obsoletas desde v1.6
- ✅ **Código más limpio** - Mejor mantenibilidad del código

### ⚡ Optimizaciones de rendimiento

- ✅ **Polling reducido** - Actualización de switches cada 15 segundos en lugar de cada 2 segundos (87.5% menos carga)
- ✅ **Validación de límites** - El frontend valida el límite de 80 horarios antes de agregar nuevos

### 📝 Características de v1.6 (mantenidas)

- ✅ **Switches por día de la semana** - Activa o desactiva todos los horarios de Lunes, Martes, Miércoles, etc.
- ✅ **Switch maestro global** - Activa o desactiva TODOS los horarios del sistema de una vez
- ✅ **Vista consolidada** - Los horarios que suenan a la misma hora se agrupan en una sola línea mostrando todos los timbres
- ✅ **Interfaz organizada por días** - Cada día tiene su propia sección con icono distintivo y contador de horarios
- ✅ **Switches por día en HA** - Control de cada día de la semana desde Home Assistant
- ✅ **Sincronización mejorada** - Sincronización bidireccional entre HA y la interfaz web

## Integración MQTT / Home Assistant

Esta versión incluye una arquitectura simplificada para MQTT:

### Entidades disponibles en Home Assistant

**Switches de horarios por día:**
- `switch.horarios_lunes` - Activar/desactivar todos los horarios del Lunes
- `switch.horarios_martes` - Activar/desactivar todos los horarios del Martes
- `switch.horarios_miercoles` - Activar/desactivar todos los horarios del Miércoles
- `switch.horarios_jueves` - Activar/desactivar todos los horarios del Jueves
- `switch.horarios_viernes` - Activar/desactivar todos los horarios del Viernes
- `switch.horarios_sabado` - Activar/desactivar todos los horarios del Sábado
- `switch.horarios_domingo` - Activar/desactivar todos los horarios del Domingo

**Switch maestro:**
- `switch.horarios_programados` - Switch maestro que activa/desactiva TODOS los horarios del sistema

**Botones (para activación manual):**
- `button.timbre_1` - Activar Timbre 1 manualmente
- `button.timbre_2` - Activar Timbre 2 manualmente
- `button.timbre_3` - Activar Timbre 3 manualmente
- `button.timbre_4` - Activar Timbre 4 manualmente

**Sensores:**
- `sensor.timbres_wifi_signal` - Señal WiFi del ESP32 (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

### Topics MQTT

**Control de horarios por día:**
- `timbres/schedules/day0/set` - Activar/desactivar Lunes (payload: `ON`/`OFF`)
- `timbres/schedules/day1/set` - Activar/desactivar Martes (payload: `ON`/`OFF`)
- `timbres/schedules/day2/set` - Activar/desactivar Miércoles (payload: `ON`/`OFF`)
- `timbres/schedules/day3/set` - Activar/desactivar Jueves (payload: `ON`/`OFF`)
- `timbres/schedules/day4/set` - Activar/desactivar Viernes (payload: `ON`/`OFF`)
- `timbres/schedules/day5/set` - Activar/desactivar Sábado (payload: `ON`/`OFF`)
- `timbres/schedules/day6/set` - Activar/desactivar Domingo (payload: `ON`/`OFF`)
- `timbres/schedules/set` - Switch maestro global (payload: `ON`/`OFF`)

**Control de timbres (activación manual):**
- `timbres/bell0/set` - Activar Timbre 1 (payload: `ON`)
- `timbres/bell1/set` - Activar Timbre 2 (payload: `ON`)
- `timbres/bell2/set` - Activar Timbre 3 (payload: `ON`)
- `timbres/bell3/set` - Activar Timbre 4 (payload: `ON`)

**Sensores:**
- `timbres/wifi/rssi` - Señal WiFi (actualizado cada 30 segundos)
- `timbres/wifi/ip` - Dirección IP (actualizado cada 30 segundos)

## Archivos incluidos

- `tora-or-combined-v1.6.bin` - **NUEVO**: Archivo combinado con todo (firmware + filesystem + bootloader + particiones) - La forma más fácil de flashear
- `tora-or-firmware-v1.6.bin` - Firmware principal del sistema
- `tora-or-filesystem-v1.6.bin` - Sistema de archivos con páginas web
- `bootloader.bin` - Bootloader del ESP32
- `partitions.bin` - Tabla de particiones

## Instrucciones de instalación

### Opción 1: Actualización OTA desde la interfaz web (MÁS FÁCIL) ⭐

Si ya tienes una versión anterior instalada:

1. **IMPORTANTE**: Primero crea un backup desde la página de configuración
2. Accede a la interfaz web del sistema (http://IP_DEL_ESP32/ota)
3. La página de actualización OTA está disponible sin autenticación
4. Selecciona `tora-or-firmware-v1.6.bin`
5. Haz clic en "Actualizar"
6. Espera a que termine (el ESP32 se reiniciará automáticamente)
7. **Para actualizar el filesystem** (recomendado):
   - Accede nuevamente a http://IP_DEL_ESP32/ota
   - Esta vez selecciona `tora-or-filesystem-v1.6.bin`
   - Haz clic en "Actualizar Filesystem"
   - El ESP32 se reiniciará y verás la nueva interfaz

**Nota:** Si solo actualizas el firmware, seguirás viendo la interfaz antigua. Para ver la nueva interfaz consolidada, debes actualizar el filesystem también.

### Opción 2: ESPHome Web (INSTALACIÓN LIMPIA)

1. Ve a https://web.esphome.io/
2. Conecta el ESP32 al puerto USB
3. Haz clic en "Connect"
4. Selecciona tu puerto COM
5. Haz clic en "Install" → "Manual installation"
6. Sube los archivos en este orden:
   - `0x1000`: `bootloader.bin`
   - `0x8000`: `partitions.bin`
   - `0x10000`: `tora-or-firmware-v1.6.bin`
   - `0x290000`: `tora-or-filesystem-v1.6.bin`

### Opción 3: Línea de comandos - Archivo combinado (MÁS RÁPIDO) ⚡

**Esta es la forma más rápida de flashear todo el sistema de una vez.**

#### Requisitos
- ESP32 Dev Module
- Cable USB
- Python con esptool instalado

#### Instalación con archivo combinado

1. Conecta el ESP32 al puerto USB

2. Pon el ESP32 en modo de descarga:
   - Mantén presionado el botón BOOT
   - Conecta el USB (o presiona RESET si ya está conectado)
   - Mantén BOOT por 3-5 segundos más
   - Suelta BOOT

3. Ejecuta el siguiente comando (ajusta COM4 a tu puerto):

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x0 tora-or-combined-v1.6.bin
```

**Ventajas:**
- ✅ Un solo comando
- ✅ Un solo archivo
- ✅ Flasheo más rápido
- ✅ Menos probabilidad de error

### Opción 4: Línea de comandos - Archivos separados (instalación completa)

Si prefieres flashear los archivos por separado:

1. Conecta el ESP32 al puerto USB

2. Pon el ESP32 en modo de descarga:
   - Mantén presionado el botón BOOT
   - Conecta el USB (o presiona RESET si ya está conectado)
   - Mantén BOOT por 3-5 segundos más
   - Suelta BOOT

3. Ejecuta el siguiente comando (ajusta COM4 a tu puerto):

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 tora-or-firmware-v1.6.bin 0x290000 tora-or-filesystem-v1.6.bin
```

### Opción 5: Solo actualización de firmware y filesystem

Si solo quieres actualizar el firmware y filesystem sin tocar bootloader ni particiones:

```bash
python -m esptool --chip esp32 --port COM4 --baud 921600 write_flash -z 0x10000 tora-or-firmware-v1.6.bin 0x290000 tora-or-filesystem-v1.6.bin
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

### Republicar Discovery (si los switches no aparecen)

Si los switches no aparecen correctamente en Home Assistant:

1. En la página de configuración del sistema (http://IP_DEL_ESP32/config.html)
2. Baja hasta la sección "Integración MQTT / Home Assistant"
3. Click en el botón **"🔄 Republicar Discovery"**
4. Espera 10 segundos
5. Verifica en Home Assistant que los dispositivos se hayan actualizado

## Actualización desde versión anterior

### Desde v1.5, v1.4, v1.3 o anterior:

1. **IMPORTANTE**: Primero crea un backup desde la página de configuración (requiere login como admin)
2. **Instalar v1.6**: Usa el método OTA o cualquiera de los métodos de instalación arriba
   - **NUEVO EN v1.6**: El OTA ahora requiere login como administrador
   - Si usas OTA, actualiza tanto firmware como filesystem
3. **Restaurar backup**: Ve a Configuración → Backup y Restauración → Restaurar Backup
   - **NUEVO EN v1.6**: La descarga de backup requiere login como administrador
   - El backup restaurará toda tu configuración incluyendo MQTT
   - Los horarios programados se conservarán

### ¿Qué se conserva al actualizar?

- ✅ Nombres de timbres personalizados
- ✅ Duraciones de timbres
- ✅ Horarios programados
- ✅ Configuración WiFi
- ✅ Configuración MQTT
- ✅ Usuario normal (si está configurado)
- ✅ Visibilidad de timbres

### ⚠️ Cambios de seguridad en v1.6

**Autenticación requerida:**
- **OTA ahora requiere admin**: Para actualizar el firmware vía OTA debes iniciar sesión como administrador primero
- **Backup ahora requiere admin**: Para descargar backups debes iniciar sesión como administrador
- Estas medidas protegen tu sistema de accesos no autorizados y protegen credenciales WiFi/MQTT

**Si vienes desde v1.5:**
- No hay cambios en la interfaz ni en MQTT
- Simplemente actualiza y disfruta de las mejoras de seguridad y rendimiento

**Si vienes desde v1.4 o anterior:**
- Se eliminarán los switches individuales por timbre (`switch.horarios_timbre_1`, etc.)
- Se crearán nuevos switches por día de la semana (`switch.horarios_lunes`, etc.)
- Si tenías automatizaciones que usaban switches de timbres, necesitarás actualizarlas
- Migración de automatizaciones:
  - **Antes**: Para desactivar horarios del Timbre 1 → `switch.horarios_timbre_1`
  - **Ahora**: Para desactivar horarios de un día → `switch.horarios_lunes`

## Uso de la nueva interfaz

### Vista por días

La interfaz ahora organiza los horarios por días de la semana:
- Cada día tiene un icono distintivo y un color
- Puedes ver cuántos horarios hay configurados para cada día
- Cada día tiene su propio switch maestro para activar/desactivar todos sus horarios

### Horarios consolidados

Cuando varios timbres suenan a la misma hora con el mismo nombre:
- Se muestran en una sola línea
- Los nombres de los timbres se listan separados por comas
- Ejemplo: "08:00 - Timbre 1, Timbre 2, Timbre 3"

Esto simplifica la visualización cuando tienes:
- Recreo (todos los timbres a la vez)
- Entrada (varios timbres al mismo tiempo)
- Cambio de clase (múltiples timbres)

### Estados visuales

Los horarios muestran claramente su estado:
- **Verde "Activo"**: El horario está activo y sonará
- **Gris "Inactivo"**: El horario está desactivado individualmente
- **Rojo "Día desactivado"**: El día completo está desactivado

## Soporte

Para más información o para reportar problemas:
- Documentación completa: https://github.com/DiraSmart/Timbres-Tora-Or
- Guía de integración con Home Assistant: INTEGRACION_HOME_ASSISTANT.md
- Reportar problemas: https://github.com/DiraSmart/Timbres-Tora-Or/issues
