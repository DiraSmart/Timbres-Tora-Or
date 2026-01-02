# Sistema de Timbres Tora Or con ESP32

Sistema automatizado para controlar 4 timbres con programación de horarios mediante interfaz web, RTC para funcionamiento sin internet, sincronización NTP, **Captive Portal WiFi** para configuración fácil, y **soporte completo MQTT / Home Assistant** para automatización del hogar.

## Características

### 🆕 Nuevo en v1.3: Integración MQTT / Home Assistant
- **Integración MQTT completa** - Conecta con Home Assistant u otro broker MQTT
- **Auto-descubrimiento Home Assistant** - Los dispositivos aparecen automáticamente en HA
- **Switches individuales** - Control remoto de cada timbre desde Home Assistant
- **Switch global del sistema** - Activa/desactiva todo el sistema remotamente
- **Sensores de estado** - Señal WiFi y dirección IP en Home Assistant
- **Eventos de timbres** - Notificaciones cuando se activan/desactivan timbres
- **Configuración web MQTT** - Configura MQTT desde la interfaz de configuración
- **Backup incluye MQTT** - La configuración MQTT se guarda en backups

### Características principales
- **Captive Portal WiFi** - Configuración WiFi fácil en 192.168.4.1
- **Modo Access Point automático** - Si no hay WiFi configurado o pierde conexión
- **Reconexión automática** - Intenta reconectar cada 2 minutos al WiFi guardado
- **Sistema de usuarios y permisos** - Admin y usuario normal con diferentes niveles de acceso
- **Nombres personalizables** - Puedes nombrar los horarios y timbres
- **Control de visibilidad** - Oculta timbres que no estés usando
- **Backup/Restore** - Exporta e importa toda la configuración en formato JSON
- **Actualización OTA** - Actualiza el firmware sin cable y sin perder configuración
- Control de 4 relés para 4 timbres independientes
- Programación de horarios individuales para cada timbre
- **Switch global** - Activa/desactiva todos los horarios de una vez
- Selección de días de la semana para cada horario
- Duración configurable para cada timbre (1-60 segundos)
- Módulo RTC (DS3231) para mantener hora sin internet
- Configurado para zona horaria de Panamá (GMT-5)
- Sincronización automática con servidores NTP cuando hay internet
- Interfaz web intuitiva y responsive
- Almacenamiento persistente de configuraciones
- Prueba manual de cada timbre desde la web
- **Funcionamiento continuo** - Los timbres funcionan en modo AP o WiFi

## Componentes Necesarios

### Hardware

1. **ESP32 DevKit** (cualquier modelo)
2. **Módulo RTC DS3231** con batería CR2032
3. **Módulo de 4 Relés** (5V o 3.3V según tu configuración)
4. **4 Timbres** o dispositivos a controlar
5. **Fuente de alimentación** adecuada para ESP32 y relés
6. **Cables jumper** para conexiones

### Software

1. **PlatformIO** (extensión de VSCode) o **Arduino IDE**
2. **Drivers USB** para ESP32 (CP2102 o CH340 según tu placa)

## Conexiones Hardware

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

**Nota:** Puedes cambiar los pines en el archivo [src/main.cpp](src/main.cpp) en la línea:
```cpp
const int RELAY_PINS[4] = {32, 33, 25, 26};
```

### Conexión de Timbres a los Relés

Cada timbre se conecta al contacto normalmente abierto (NO) de su relé correspondiente. Consulta las especificaciones de voltaje de tus timbres.

## Instalación

### 1. Clonar o Descargar el Proyecto

```bash
cd "c:\Users\jbran\Downloads\Proyecto Tora Ohr"
```

### 2. Compilar y Subir el Código

**NOTA:** Ya NO es necesario editar el código para configurar WiFi. La configuración se hace desde el portal web.

#### Usando PlatformIO (Recomendado)

```bash
# Compilar el proyecto
pio run

# Subir al ESP32
pio run --target upload

# Ver monitor serial
pio device monitor
```

#### Usando Arduino IDE

1. Abre [src/main.cpp](src/main.cpp)
2. Instala las librerías necesarias desde el Library Manager:
   - ArduinoJson (v7.2.1 o superior)
   - RTClib por Adafruit
   - NTPClient
3. Selecciona tu placa ESP32 en Tools → Board
4. Selecciona el puerto COM correcto
5. Click en Upload

### 4. Subir el Sistema de Archivos (IMPORTANTE)

El archivo HTML debe subirse al sistema de archivos LittleFS del ESP32.

#### Con PlatformIO

```bash
pio run --target uploadfs
```

#### Con Arduino IDE

1. Instala el plugin "ESP32 Sketch Data Upload" o "LittleFS Data Upload"
2. Coloca el archivo [data/index.html](data/index.html) en la carpeta `data/`
3. Usa Tools → ESP32 Sketch Data Upload

### 5. Configuración Inicial WiFi

El ESP32 arrancará en **modo Access Point** la primera vez:

1. Abre el Monitor Serial (115200 baud)
2. Verás el mensaje:
   ```
   Modo: Access Point
   Conéctate a: Timbres-Escuela-Config
   Contraseña: 12345678
   Abre: http://192.168.4.1/config
   ```
3. Desde tu teléfono o computadora:
   - Conéctate a la red WiFi `Timbres-Escuela-Config` (contraseña: `12345678`)
   - Abre tu navegador y ve a `http://192.168.4.1/config`
   - O simplemente abre el navegador (debería redirigir automáticamente - Captive Portal)
4. En la página de configuración:
   - Click en "Escanear Redes WiFi"
   - Selecciona tu red WiFi
   - Ingresa la contraseña
   - Click en "Guardar y Conectar"
5. El ESP32 intentará conectarse a tu WiFi
6. Una vez conectado, anota la nueva IP para acceder al panel principal

## Uso del Sistema

### Acceso a la Interfaz Web

**Primera vez o sin WiFi configurado:**
1. Conéctate a la red WiFi `Timbres-Escuela-Config` (contraseña: `12345678`)
2. Abre `http://192.168.4.1/config` para configurar WiFi
3. O abre `http://192.168.4.1` para acceder directamente al panel

**Con WiFi configurado:**
1. Asegúrate de estar en la misma red WiFi que el ESP32
2. Ve a la dirección IP mostrada en el monitor serial (ejemplo: `http://192.168.1.100`)

**Nota:** Si el ESP32 pierde la conexión WiFi por más de 2 minutos, volverá automáticamente a modo Access Point.

### Configurar Horarios

1. Click en **"+ Agregar Horario"**
2. Selecciona el timbre (1-4)
3. Configura la hora (formato 24 horas)
4. Selecciona los días de la semana (L-D)
5. Click en **"Guardar"**

### Probar Timbres

En la sección superior puedes hacer click en cualquier botón de timbre para activarlo manualmente durante 3 segundos.

### Sincronizar Hora

- **Automático:** Si hay internet, se sincroniza cada hora con servidores NTP
- **Manual con Internet:** Click en "Sincronizar con Internet (NTP)"
- **Manual sin Internet:** Click en "Configurar Hora Manualmente"

### Editar o Eliminar Horarios

Cada horario muestra botones para **Editar** o **Eliminar**. Los cambios se guardan automáticamente en la memoria del ESP32.

## Funcionamiento del Sistema WiFi

### Comportamiento Automático

El sistema tiene 3 modos de funcionamiento:

1. **Modo Access Point (AP)**
   - Se activa al inicio si no hay WiFi configurado
   - Se activa automáticamente si pierde conexión por más de 2 minutos
   - Red WiFi: `Timbres-Escuela-Config` (contraseña: `12345678`)
   - IP fija: `192.168.4.1`
   - Portal cautivo activo (redirige automáticamente al abrir navegador)

2. **Modo Cliente WiFi (STA)**
   - Conectado a tu red WiFi local
   - Sincroniza hora con NTP automáticamente
   - Verifica conexión cada 5 segundos
   - IP asignada por tu router

3. **Reconexión Automática**
   - Si está en modo AP y hay WiFi guardado, intenta reconectar cada 2 minutos
   - Si está conectado y pierde WiFi, vuelve a modo AP automáticamente
   - Los timbres siguen funcionando en cualquier modo

### Cambiar Credenciales del Access Point

En [src/main.cpp:13-14](src/main.cpp#L13-L14), modifica:

```cpp
const char* ap_ssid = "Timbres-Escuela-Config";
const char* ap_password = "12345678"; // Mínimo 8 caracteres
```

### Cambiar Tiempo de Reconexión

En [src/main.cpp:28](src/main.cpp#L28), modifica:

```cpp
const unsigned long WIFI_RETRY_INTERVAL = 120000; // 2 minutos en milisegundos
// Para 1 minuto: usar 60000
// Para 5 minutos: usar 300000
```

## Integración con Home Assistant (MQTT)

### Configuración Rápida

1. **Configurar broker MQTT en Home Assistant**
   - Instala el add-on "Mosquitto broker" desde Settings → Add-ons
   - Inicia el broker y configura usuario/contraseña (opcional)

2. **Configurar MQTT en el sistema de timbres**
   - Accede a la interfaz web del ESP32
   - Inicia sesión como administrador (`dirasmart` / `dirasmart1`)
   - Ve a Configuración → Integración MQTT / Home Assistant
   - Habilita MQTT y configura:
     - **Servidor MQTT**: IP de Home Assistant (ej: `192.168.1.10`)
     - **Puerto**: `1883`
     - **Usuario/Contraseña**: Credenciales del broker (si configuraste autenticación)
     - **Auto-descubrimiento HA**: ✅ Activado
   - Guarda la configuración

3. **Verificar en Home Assistant**
   - Ve a Settings → Devices & Services → MQTT
   - Deberías ver el dispositivo "Sistema Timbres Tora Or"
   - Cada timbre aparecerá como un switch individual

### Entidades Disponibles en Home Assistant

**Switches (Interruptores):**
- `switch.timbre_1` - Control del Timbre 1
- `switch.timbre_2` - Control del Timbre 2
- `switch.timbre_3` - Control del Timbre 3
- `switch.timbre_4` - Control del Timbre 4
- `switch.sistema_timbres_global` - Switch global del sistema

**Sensores:**
- `sensor.timbres_wifi_signal` - Señal WiFi (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

### Ejemplo de Automatización en Home Assistant

```yaml
automation:
  - alias: "Timbre de Recreo a las 10:30"
    trigger:
      - platform: time
        at: "10:30:00"
    condition:
      - condition: state
        entity_id: binary_sensor.workday_sensor
        state: "on"
    action:
      - service: switch.turn_on
        target:
          entity_id: switch.timbre_2
```

### Documentación Completa

Para más información sobre la integración MQTT, consulta el archivo [INTEGRACION_HOME_ASSISTANT.md](INTEGRACION_HOME_ASSISTANT.md) que incluye:
- Topics MQTT disponibles
- Ejemplos de automatizaciones avanzadas
- Solución de problemas
- Configuración de dashboards personalizados

## Configuración Avanzada

### Cambiar Duración del Timbre

En [src/main.cpp:24](src/main.cpp#L24), modifica:

```cpp
const int BELL_DURATION = 3000; // en milisegundos (3000 = 3 segundos)
```

### Cambiar Zona Horaria

El sistema está configurado para Panamá (GMT-5). Para cambiar, edita en [src/main.cpp:30](src/main.cpp#L30):

```cpp
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000);
// -18000 = -5 horas en segundos
// Para GMT-6: usar -21600
// Para GMT-4: usar -14400
```

### Cambiar Servidor NTP

Puedes usar servidores NTP más cercanos editando:

```cpp
NTPClient timeClient(ntpUDP, "pa.pool.ntp.org", -18000, 60000);
// "pa.pool.ntp.org" para servidores en Panamá
```

### Aumentar Capacidad de Horarios

Por defecto soporta 80 horarios totales. Para cambiar, edita en [src/main.cpp:47](src/main.cpp#L47):

```cpp
const int MAX_SCHEDULES = 80; // Aumenta según necesites
```

## Solución de Problemas

### No puedo acceder al portal de configuración

1. Asegúrate de estar conectado a la red WiFi `Timbres-Escuela-Config`
2. Intenta abrir `http://192.168.4.1/config` directamente
3. Si no funciona el captive portal, escribe la URL manualmente
4. Verifica en el monitor serial que el AP esté activo

### El ESP32 no se conecta a mi WiFi

1. Verifica que la contraseña sea correcta (distingue mayúsculas/minúsculas)
2. Asegúrate que la red sea 2.4GHz (ESP32 no soporta 5GHz)
3. Revisa el monitor serial para ver mensajes de error
4. El ESP32 volverá a modo AP después de fallar

### El ESP32 sigue en modo AP aunque configuré WiFi

1. Verifica que el WiFi esté disponible y funcionando
2. Espera 2 minutos para que intente reconectar automáticamente
3. Revisa el monitor serial para ver intentos de conexión
4. Puedes reiniciar el ESP32 para forzar un nuevo intento

### Perdí la IP del ESP32 en modo WiFi

1. Revisa el monitor serial, muestra la IP al conectar
2. Revisa la lista de dispositivos en tu router
3. O conéctate al AP (ESP32 vuelve a AP si pierde WiFi)

### Los relés no activan

1. Verifica que los pines en el código coincidan con tu conexión física
2. Algunos módulos de relés son activos en BAJO, cambia:
   ```cpp
   digitalWrite(RELAY_PINS[i], HIGH); // a LOW
   digitalWrite(RELAY_PINS[i], LOW);  // a HIGH
   ```

### No carga la página web

1. Asegúrate de haber subido el sistema de archivos (uploadfs)
2. Verifica que los archivos estén en `data/index.html` y `data/wifi_config.html`
3. Reinicia el ESP32 después de subir los archivos
4. En modo AP, intenta `http://192.168.4.1` o `http://192.168.4.1/config`

### El RTC no mantiene la hora

1. Verifica que la batería CR2032 esté instalada y con carga
2. Revisa las conexiones SDA/SCL
3. Verifica en el monitor serial si detecta el RTC

### Los horarios no se activan

1. Verifica que la hora del sistema sea correcta
2. Asegúrate que los días de la semana estén seleccionados
3. Revisa que el horario esté habilitado (enabled)
4. El sistema verifica cada minuto, espera al cambio de minuto

## Estructura del Proyecto

```
Proyecto Tora Ohr/
├── platformio.ini          # Configuración de PlatformIO
├── src/
│   └── main.cpp           # Código principal del ESP32
├── data/
│   ├── index.html         # Interfaz web principal (panel de control)
│   └── wifi_config.html   # Interfaz de configuración WiFi
├── README.md              # Este archivo
└── DIAGRAMA_CONEXIONES.txt # Diagrama de conexiones hardware
```

## API REST

El sistema expone los siguientes endpoints:

**Configuración WiFi:**
- `GET /config` - Portal de configuración WiFi
- `GET /api/wifi/scan` - Escanear redes WiFi disponibles
- `POST /api/wifi/save` - Guardar configuración WiFi (JSON)
- `GET /api/wifi/status` - Estado actual de conexión WiFi

**Panel Principal:**
- `GET /` - Interfaz web principal
- `GET /api/schedules` - Obtener todos los horarios
- `POST /api/schedules` - Guardar horarios (JSON array)
- `GET /api/time` - Obtener hora actual del RTC
- `POST /api/time` - Configurar hora del RTC (JSON)
- `POST /api/test?bell=[0-3]` - Probar un timbre
- `POST /api/sync` - Sincronizar con NTP

## Formato JSON de Horarios

```json
[
  {
    "enabled": true,
    "hour": 7,
    "minute": 30,
    "bell": 0,
    "days": [true, true, true, true, true, false, false]
  }
]
```

Donde `days` es un array de 7 booleanos: [Lun, Mar, Mié, Jue, Vie, Sáb, Dom]

## Licencia

Este proyecto es de código abierto y libre para uso educativo.

## Soporte

Para problemas o preguntas, revisa:
1. El monitor serial para mensajes de debug
2. La consola del navegador (F12) para errores web
3. Las conexiones físicas del hardware

## Mejoras Futuras

- ✅ ~~Modo Access Point para configuración inicial sin WiFi~~ (Implementado v1.0)
- ✅ ~~Captive Portal~~ (Implementado v1.0)
- ✅ ~~Reconexión automática~~ (Implementado v1.0)
- ✅ ~~Integración MQTT / Home Assistant~~ (Implementado v1.3)
- ✅ ~~Auto-descubrimiento Home Assistant~~ (Implementado v1.3)
- Notificaciones push desde Home Assistant
- Historial de activaciones en base de datos
- Soporte para más de 4 timbres
- Configuración de melodías diferentes por timbre
- Integración con Google Calendar
- Modo AP+STA simultáneo permanente
- Página de diagnóstico y logs
- Dashboard de estadísticas de uso
