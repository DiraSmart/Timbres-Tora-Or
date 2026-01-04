# Changelog - v1.4

## [1.4.0] - 2026-01-03

### ✨ Nuevas funcionalidades

#### MQTT / Home Assistant
- **Master schedule switch fix**: El switch maestro de horarios programados ahora actualiza correctamente todos los switches individuales cuando se cambia desde Home Assistant
- **Sincronización bidireccional mejorada**: Mejor sincronización de estados entre Home Assistant y la interfaz web del ESP32
- **Feedback visual**: Los horarios programados se muestran en gris cuando están desactivados desde el switch maestro

#### Interfaz de usuario
- **Iconos uniformes**:
  - Todos los switches de horarios usan `mdi:calendar-clock`
  - Todos los botones de prueba de timbres usan `mdi:bell-ring`
- **Nombre de dispositivo actualizado**: Cambio de "ESP32 Bell Controller" a "Bell Controller" en Home Assistant

### 🐛 Correcciones de errores

#### Sistema de Backup/Restore
- **Fix error "archivo inválido"**: Corregido el mensaje de error que aparecía al restaurar un backup exitosamente
- **Mejora en reconexión**: La reconexión WiFi y MQTT ahora ocurre DESPUÉS de enviar la respuesta HTTP, evitando que la respuesta se corte o llegue incompleta al navegador

#### Frontend
- **Mejor manejo de errores**: Agregada verificación de estado HTTP (`response.ok`) antes de intentar parsear JSON en la función de restore
- **Mensajes de error más claros**: Mejor identificación de errores HTTP vs errores de parsing

### 🔧 Cambios técnicos

#### Backend (src/main.cpp)
- Movida la reconexión WiFi a ejecutarse DESPUÉS de `server.send()` en `handleRestore()`
- Movida la reconexión MQTT a ejecutarse DESPUÉS de `server.send()` en `handleRestore()`
- Actualizada versión de software de "1.3" a "1.4" en todos los mensajes de discovery MQTT
- Actualizada versión de backup de "1.0" a "1.4"
- Cambiado `doc["device"]["model"]` de "ESP32 Bell Controller" a "Bell Controller" (5 ocurrencias)
- Modificado MQTT callback para `timbres/schedules/set` para cascadear cambios a todos los `bellSchedulesEnabled[i]`

#### Frontend (data/config.html)
- Agregado check de `response.ok` antes de `response.json()` en función `restoreBackup()`
- Mejorado manejo de errores HTTP en restore

#### MQTT Discovery
- Todos los switches de horarios ahora usan `"icon": "mdi:calendar-clock"`
- Todos los botones de timbres ahora usan `"icon": "mdi:bell-ring"`

### 📝 Documentación

- Actualizado INSTRUCCIONES.md para v1.4
- Agregadas notas sobre la función "Republicar Discovery"
- Documentadas las mejoras en la sincronización MQTT

### 🔄 Cambios desde v1.3

**Archivos modificados:**
- `src/main.cpp`: Mejoras en MQTT y restore
- `data/config.html`: Fix en manejo de errores de restore
- `data/index.html`: Actualización de sincronización de switches (sin cambios en esta versión)

**Compatibilidad:**
- ✅ Compatible con backups de v1.3, v1.2, v1.1 y v1.0
- ✅ No requiere reconfiguración de MQTT
- ✅ Los dispositivos existentes en Home Assistant se actualizan automáticamente

**Actualización recomendada:**
- Si usas MQTT/Home Assistant y experimentas problemas con la sincronización del switch maestro de horarios, se recomienda actualizar a v1.4
- Si experimentas el error "archivo inválido" al restaurar backups, actualizar a v1.4 resolverá el problema

### 🎯 Próximos pasos (futuras versiones)

- Considerando agregar más sensores de estado a Home Assistant
- Explorando la posibilidad de configurar horarios desde Home Assistant
- Mejoras en la interfaz de usuario para visualización de estados

---

**Desarrollado con [Claude Code](https://claude.com/claude-code)**

Co-Authored-By: Claude Sonnet 4.5 <noreply@anthropic.com>
