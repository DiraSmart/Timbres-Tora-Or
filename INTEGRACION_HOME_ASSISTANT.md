# Integración con Home Assistant

Este documento describe cómo integrar el Sistema de Timbres Tora Or con Home Assistant usando MQTT.

## Requisitos Previos

1. **Home Assistant** instalado y funcionando
2. **Broker MQTT** configurado en Home Assistant (por ejemplo: Mosquitto)
3. **Sistema Timbres Tora Or v1.3** o superior con soporte MQTT

## Configuración del Broker MQTT

### Opción 1: Usando Mosquitto (Recomendado)

1. En Home Assistant, ve a **Settings → Add-ons**
2. Busca e instala **Mosquitto broker**
3. Inicia el add-on y habilita "Start on boot"
4. Toma nota de las credenciales (usuario/contraseña)

### Opción 2: Broker MQTT Externo

Si usas un broker MQTT externo, asegúrate de tener:
- Dirección IP o hostname del broker
- Puerto (usualmente 1883)
- Credenciales de acceso (opcional)

## Configurar MQTT en el Sistema de Timbres

### Desde la Interfaz Web

1. Accede a la interfaz web del sistema (http://IP_DEL_ESP32)
2. Inicia sesión como administrador (usuario: `dirasmart`, contraseña: `dirasmart1`)
3. Ve a la página de **Configuración**
4. En la sección **MQTT**, configura:
   - **Servidor MQTT**: IP de tu broker (ej: `192.168.1.10` o `homeassistant.local`)
   - **Puerto**: `1883` (por defecto)
   - **Usuario**: Tu usuario MQTT (si aplica)
   - **Contraseña**: Tu contraseña MQTT (si aplica)
   - **Client ID**: `timbres-tora-or` (puedes dejarlo por defecto)
   - **Habilitado**: ✅ Activar
   - **Auto-descubrimiento HA**: ✅ Activar
5. Click en **Guardar Configuración MQTT**

El sistema se conectará automáticamente al broker MQTT.

## Auto-Descubrimiento en Home Assistant

Con la opción "Auto-descubrimiento HA" activada, el sistema publicará automáticamente su configuración a Home Assistant. No necesitas configurar nada manualmente.

### Entidades que se crean automáticamente:

#### Switches (Interruptores)
- `switch.timbre_1` - Control del Timbre 1
- `switch.timbre_2` - Control del Timbre 2
- `switch.timbre_3` - Control del Timbre 3
- `switch.timbre_4` - Control del Timbre 4
- `switch.sistema_timbres_global` - Switch global para activar/desactivar todo el sistema

#### Sensores
- `sensor.timbres_wifi_signal` - Señal WiFi del ESP32 (dBm)
- `sensor.timbres_ip_address` - Dirección IP del ESP32

### Verificar la Integración

1. Ve a **Settings → Devices & Services** en Home Assistant
2. Busca "MQTT" y abre la integración
3. Deberías ver un dispositivo llamado **"Sistema Timbres Tora Or"**
4. Click en el dispositivo para ver todas las entidades disponibles

## Uso de las Entidades

### Activar un Timbre Manualmente

Desde la interfaz de Home Assistant:
```yaml
# En el dashboard, agrega una tarjeta de entidades
type: entities
entities:
  - switch.timbre_1
  - switch.timbre_2
  - switch.timbre_3
  - switch.timbre_4
  - switch.sistema_timbres_global
```

### Crear Automatizaciones

#### Ejemplo 1: Activar timbre al presionar un botón

```yaml
automation:
  - alias: "Activar Timbre 1 con botón"
    trigger:
      - platform: state
        entity_id: input_boolean.activar_timbre
        to: "on"
    action:
      - service: switch.turn_on
        target:
          entity_id: switch.timbre_1
```

#### Ejemplo 2: Activar timbre según horario (alternativa a horarios programados)

```yaml
automation:
  - alias: "Timbre de Recreo"
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

#### Ejemplo 3: Desactivar sistema en vacaciones

```yaml
automation:
  - alias: "Desactivar Timbres en Vacaciones"
    trigger:
      - platform: state
        entity_id: calendar.school_holidays
        to: "on"
    action:
      - service: switch.turn_off
        target:
          entity_id: switch.sistema_timbres_global
```

#### Ejemplo 4: Notificación cuando se activa un timbre

```yaml
automation:
  - alias: "Notificar activación de timbre"
    trigger:
      - platform: mqtt
        topic: "timbres/bell0/event"
        payload: "activated"
    action:
      - service: notify.mobile_app
        data:
          message: "El Timbre 1 ha sido activado"
          title: "Sistema de Timbres"
```

## Topics MQTT Disponibles

### Topics de Control (Comandos)

| Topic | Descripción | Payload |
|-------|-------------|---------|
| `timbres/bell0/set` | Activar Timbre 1 | `ON` |
| `timbres/bell1/set` | Activar Timbre 2 | `ON` |
| `timbres/bell2/set` | Activar Timbre 3 | `ON` |
| `timbres/bell3/set` | Activar Timbre 4 | `ON` |
| `timbres/system/set` | Activar/Desactivar Sistema | `ON` / `OFF` |

### Topics de Estado (Publicados por el sistema)

| Topic | Descripción | Actualización |
|-------|-------------|---------------|
| `timbres/bell0/state` | Estado del Timbre 1 | Al activar/desactivar |
| `timbres/bell1/state` | Estado del Timbre 2 | Al activar/desactivar |
| `timbres/bell2/state` | Estado del Timbre 3 | Al activar/desactivar |
| `timbres/bell3/state` | Estado del Timbre 4 | Al activar/desactivar |
| `timbres/bell0/event` | Eventos del Timbre 1 | `activated` / `deactivated` |
| `timbres/bell1/event` | Eventos del Timbre 2 | `activated` / `deactivated` |
| `timbres/bell2/event` | Eventos del Timbre 3 | `activated` / `deactivated` |
| `timbres/bell3/event` | Eventos del Timbre 4 | `activated` / `deactivated` |
| `timbres/system/state` | Estado del sistema global | `ON` / `OFF` |
| `timbres/wifi/rssi` | Señal WiFi | Cada 30 segundos |
| `timbres/wifi/ip` | Dirección IP | Cada 30 segundos |

## Solución de Problemas

### El dispositivo no aparece en Home Assistant

**Paso 1: Verificar configuración MQTT en Home Assistant**

Asegúrate de que Home Assistant tiene MQTT discovery habilitado. En `configuration.yaml`:

```yaml
mqtt:
  discovery: true
  discovery_prefix: homeassistant
```

Después de agregar o modificar esto, **reinicia Home Assistant**.

**Paso 2: Verificar configuración en el Sistema de Timbres**

1. Accede a http://IP_DEL_ESP32/config.html
2. En la sección MQTT, verifica que:
   - ✅ **Habilitar MQTT** esté marcado
   - ✅ **Auto-descubrimiento Home Assistant** esté marcado
   - El **Servidor MQTT** sea la IP correcta de Home Assistant
   - El **Puerto** sea 1883
   - El estado muestre "✅ MQTT Conectado a [IP]"

**Paso 3: Republicar el Discovery**

Si ya está todo configurado pero los dispositivos no aparecen:

1. En la página de configuración del sistema (http://IP_DEL_ESP32/config.html)
2. Baja hasta la sección "Integración MQTT / Home Assistant"
3. Click en el botón **"🔄 Republicar Discovery"**
4. Espera 10 segundos
5. En Home Assistant, ve a **Settings → Devices & Services → MQTT**
6. Busca el dispositivo "Sistema Timbres Tora Or"

**Paso 4: Verificar topics MQTT (método avanzado)**

Puedes usar una herramienta MQTT client (como MQTT Explorer) para verificar que los mensajes se están publicando:

1. Conecta MQTT Explorer al broker
2. Busca los topics bajo `homeassistant/switch/timbres/`
3. Deberías ver:
   - `homeassistant/switch/timbres/bell0/config`
   - `homeassistant/switch/timbres/bell1/config`
   - `homeassistant/switch/timbres/bell3/config` (si el timbre 2 está oculto)
   - `homeassistant/switch/timbres/system/config`
   - `homeassistant/sensor/timbres/wifi_signal/config`
   - `homeassistant/sensor/timbres/ip_address/config`

**Paso 5: Revisar logs de Home Assistant**

1. Ve a **Settings → System → Logs**
2. Busca mensajes relacionados con "mqtt" o "timbres"
3. Si ves errores de JSON inválido, contacta al desarrollador

**Paso 6: Reiniciar servicios**

Como último recurso:
1. Reinicia el ESP32 (desconecta y reconecta la alimentación)
2. Reinicia Home Assistant
3. Espera 1 minuto para que se conecten
4. Verifica de nuevo en **Settings → Devices & Services → MQTT**

### Los switches no funcionan

1. Verifica la conexión MQTT en la interfaz web (debería decir "Conectado")
2. Asegúrate de que el "Sistema Global" esté activado (ON)
3. Revisa el monitor serial del ESP32 para ver mensajes de error
4. Verifica las credenciales MQTT (usuario/contraseña)

### El ESP32 no se conecta al broker MQTT

1. Verifica que la IP del broker sea correcta y accesible desde la red del ESP32
2. Comprueba que el puerto 1883 esté abierto
3. Si usas autenticación, verifica usuario y contraseña
4. Asegúrate de que el ESP32 tenga conexión WiFi activa

## Integración Avanzada

### Crear un Dashboard Personalizado

```yaml
title: Control de Timbres Escolares
views:
  - title: Principal
    cards:
      - type: entities
        title: Control Manual de Timbres
        entities:
          - entity: switch.timbre_1
            name: "Timbre Principal"
          - entity: switch.timbre_2
            name: "Timbre Recreo"
          - entity: switch.timbre_3
            name: "Timbre Emergencia"
          - entity: switch.timbre_4
            name: "Timbre Evacuación"

      - type: entities
        title: Estado del Sistema
        entities:
          - entity: switch.sistema_timbres_global
            name: "Sistema Activo"
          - entity: sensor.timbres_wifi_signal
            name: "Señal WiFi"
          - entity: sensor.timbres_ip_address
            name: "Dirección IP"

      - type: history-graph
        title: Historial de Activaciones
        entities:
          - switch.timbre_1
          - switch.timbre_2
          - switch.timbre_3
          - switch.timbre_4
        hours_to_show: 24
```

### Script para Secuencia de Timbres

```yaml
script:
  secuencia_evacuacion:
    alias: "Secuencia de Evacuación"
    sequence:
      - service: switch.turn_on
        target:
          entity_id: switch.timbre_4
      - delay: "00:00:05"
      - service: switch.turn_on
        target:
          entity_id: switch.timbre_4
      - delay: "00:00:05"
      - service: switch.turn_on
        target:
          entity_id: switch.timbre_4
```

## Notas Importantes

- Los timbres solo se activarán si el "Sistema Global" está en ON
- Los timbres se desactivan automáticamente según la duración configurada en el sistema
- Los horarios programados en el sistema siguen funcionando independientemente de Home Assistant
- La integración MQTT no interfiere con la funcionalidad normal del sistema
- El auto-descubrimiento se publica cada vez que el ESP32 se conecta al broker MQTT

## Soporte

Para más información o problemas con la integración MQTT:
- Revisa los logs del ESP32 en el monitor serial (115200 baud)
- Consulta la documentación de Home Assistant: https://www.home-assistant.io/integrations/mqtt/
- Reporta problemas en: https://github.com/DiraSmart/Timbres-Tora-Or/issues
