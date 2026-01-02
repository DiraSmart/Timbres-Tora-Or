# Changelog - Versión 1.3

## Fecha de Lanzamiento
1 de Enero, 2026

## Cambios Principales

### 🆕 Integración MQTT / Home Assistant

#### Características nuevas:
- **Soporte completo MQTT**: Conexión a cualquier broker MQTT compatible
- **Auto-descubrimiento Home Assistant**: Los dispositivos se detectan automáticamente en HA
- **Switches individuales por timbre**: Control remoto de cada uno de los 4 timbres
- **Switch global del sistema**: Activar/desactivar todo el sistema de forma remota
- **Sensores de estado**: Publicación de señal WiFi (RSSI) y dirección IP
- **Eventos de timbres**: Notificaciones cuando los timbres se activan/desactivan
- **Configuración desde interfaz web**: Panel completo de configuración MQTT en la página de configuración
- **Backup incluye MQTT**: La configuración MQTT se guarda y restaura en backups

#### Entidades creadas automáticamente en Home Assistant:
- `switch.timbre_1` - Control del Timbre 1
- `switch.timbre_2` - Control del Timbre 2
- `switch.timbre_3` - Control del Timbre 3
- `switch.timbre_4` - Control del Timbre 4
- `switch.sistema_timbres_global` - Switch global del sistema
- `sensor.timbres_wifi_signal` - Señal WiFi (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

#### Topics MQTT:
- Control: `timbres/bell[0-3]/set` (payload: `ON`)
- Estado: `timbres/bell[0-3]/state` (payload: `ON`/`OFF`)
- Eventos: `timbres/bell[0-3]/event` (payload: `activated`/`deactivated`)
- Sistema: `timbres/system/set` y `timbres/system/state`
- Sensores: `timbres/wifi/rssi` y `timbres/wifi/ip`

### Mejoras en Interfaz Web

#### Página de Configuración:
- Nueva sección "Integración MQTT / Home Assistant"
- Campos de configuración:
  - Habilitar/deshabilitar MQTT
  - Servidor MQTT (IP o hostname)
  - Puerto MQTT (por defecto 1883)
  - Usuario MQTT (opcional)
  - Contraseña MQTT (opcional)
  - Client ID (personalizable)
  - Auto-descubrimiento Home Assistant (activable)
- Indicador de estado en tiempo real (Conectado/Desconectado/Deshabilitado)
- Auto-actualización del estado cada 5 segundos
- Protección contra borrado de campos mientras se edita
- Enlace a documentación completa de integración

#### Backup/Restore:
- Ahora incluye configuración MQTT completa
- Servidor, puerto, usuario y contraseña se guardan en el backup
- Restauración automática de configuración MQTT desde backup

### Cambios Técnicos

#### Dependencias:
- Agregada librería `PubSubClient` v2.8 para comunicación MQTT
- Actualizado `platformio.ini` con nueva dependencia

#### Backend (main.cpp):
- Nuevas funciones de manejo MQTT:
  - `loadMQTTConfig()` - Carga configuración desde LittleFS
  - `saveMQTTConfig()` - Guarda configuración a LittleFS
  - `mqttCallback()` - Maneja mensajes MQTT entrantes
  - `publishMQTTDiscovery()` - Publica auto-descubrimiento a HA
  - `reconnectMQTT()` - Reconexión automática al broker
  - `handleMQTTConnection()` - Loop de conexión MQTT
  - `publishMQTTSensors()` - Publica sensores cada 30 segundos
  - `publishBellEvent()` - Publica eventos de timbres
- Nuevos endpoints API:
  - `GET /api/mqtt/config` - Obtener configuración MQTT
  - `POST /api/mqtt/config` - Guardar configuración MQTT
- Integración con sistema de backup/restore existente
- Reconexión automática MQTT cada 5 segundos si se pierde conexión
- Publicación de sensores cada 30 segundos cuando está conectado

#### Frontend (config.html):
- Nuevo formulario de configuración MQTT
- JavaScript para cargar/guardar configuración
- Indicador visual de estado de conexión
- Validación de campos requeridos
- Auto-refresh de estado cada 5 segundos

### Documentación

#### Archivos nuevos:
- `INTEGRACION_HOME_ASSISTANT.md` - Guía completa de integración con Home Assistant
  - Requisitos previos
  - Configuración paso a paso
  - Ejemplos de automatizaciones
  - Solución de problemas
  - Integración avanzada con dashboards personalizados
  - Scripts y secuencias de ejemplo

#### Archivos actualizados:
- `README.md` - Agregada sección de integración MQTT
- `releases/v1.3/INSTRUCCIONES.md` - Instrucciones de instalación v1.3

### Compatibilidad

#### Actualización desde versiones anteriores:
- ✅ Backups de v1.0, v1.1, v1.2 son compatibles
- ✅ La configuración existente se mantiene al actualizar
- ✅ MQTT es opcional - el sistema funciona sin configurarlo
- ✅ No se requiere reconfigurar WiFi o horarios

#### Requisitos de hardware:
- Sin cambios respecto a v1.2
- Relés en GPIO 32, 33, 25, 26

### Tamaño de Firmware

- Firmware: 909 KB (±901 KB compilado)
- Filesystem: 1.4 MB
- Binario completo: 4.0 MB
- RAM usada: 51,036 bytes (15.6%)
- Flash usada: 923,645 bytes (70.5%)

### Notas de Desarrollo

#### Advertencias del compilador:
- Algunas advertencias sobre `containsKey()` deprecado en ArduinoJson v7
- No afecta la funcionalidad
- Se mantendrá compatibilidad hasta migración a nueva API

### Archivos de Release

- ✅ `tora-or-complete-v1.3.bin` - Binario unificado (RECOMENDADO)
- ✅ `tora-or-firmware-v1.3.bin` - Solo firmware
- ✅ `tora-or-filesystem-v1.3.bin` - Solo filesystem
- ✅ `bootloader.bin` - Bootloader ESP32
- ✅ `partitions.bin` - Tabla de particiones
- ✅ `INSTRUCCIONES.md` - Guía de instalación
- ✅ `CHANGELOG.md` - Este archivo

### Créditos

Desarrollado por **DiraSmart**
Sistema de Timbres Tora Or v1.3

---

Para más información, visita: https://github.com/DiraSmart/Timbres-Tora-Or
