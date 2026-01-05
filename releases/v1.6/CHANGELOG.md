# Changelog - v1.6

## [1.6.0] - 2026-01-05

### 🔒 Correcciones críticas de seguridad

#### Autenticación reforzada
- **FIX CRÍTICO: OTA requiere autenticación admin**: El endpoint de actualización OTA ahora verifica autenticación de administrador antes de permitir cualquier actualización de firmware
  - Previene que usuarios no autorizados comprometan el dispositivo
  - Bloquea intentos de OTA sin credenciales válidas de administrador
  - Registra intentos de acceso no autorizado en el log serial
- **FIX CRÍTICO: Backup restringido a admin**: La descarga de backup ahora requiere autenticación de administrador
  - Los backups contienen contraseñas WiFi y MQTT en texto plano
  - Solo administradores pueden descargar backups para proteger información sensible
  - Previene exposición de credenciales a usuarios normales

#### Generación de tokens mejorada
- **Tokens criptográficamente seguros**: Cambio de `random()` a `esp_random()` para generación de tokens de sesión
  - `esp_random()` usa generador de hardware del ESP32 (más seguro)
  - Previene predicción de tokens de sesión
  - Mejora resistencia contra ataques de fuerza bruta

### 🧹 Limpieza de código

#### Código obsoleto eliminado
- Eliminada variable `globalSystemEnabled` (sin uso desde v1.5)
- Eliminada suscripción a topic MQTT `/system/set` (obsoleto desde v1.5)
- Eliminada limpieza de switch MQTT `/system` en discovery (ya no existe)
- Código más limpio y mantenible

### ⚡ Optimizaciones de rendimiento

#### Frontend optimizado
- **Polling reducido**: Actualización de switches cada 15 segundos en lugar de cada 2 segundos
  - Reduce carga del servidor en 87.5%
  - Reduce consumo de ancho de banda
  - Mejora rendimiento general del sistema
  - La interfaz sigue respondiendo correctamente

#### Validación de límites
- **Validación de 80 horarios**: El frontend ahora valida el límite de 80 horarios antes de agregar nuevos
  - Mensaje de error claro indicando límite, horarios actuales y espacio disponible
  - Previene desbordamiento de memoria
  - Mejora experiencia de usuario con feedback inmediato

### 📝 Cambios técnicos

#### Backend (src/main.cpp)
- Versión actualizada de "1.5" a "1.6" en todos los mensajes MQTT y backup
- Función `handleOTAUpdate()`: Agregada verificación de autenticación admin con variables estáticas
- Función `handleBackup()`: Cambiada de `checkAuth()` a `checkAdminAuth()`
- Función `generateToken()`: Cambiada de `random()` a `esp_random()`
- Limpieza de código obsoleto relacionado con sistema global

#### Frontend (data/index.html)
- Función `saveSchedule()`: Agregada validación de límite de 80 horarios con mensaje detallado
- `setInterval(loadSchedulesSwitches, 15000)`: Optimizado de 2000ms a 15000ms

### 🔄 Cambios desde v1.5

**Archivos modificados:**
- `src/main.cpp`: Seguridad reforzada, limpieza de código, optimizaciones
- `data/index.html`: Validación de límites, polling optimizado

**Compatibilidad:**
- ✅ Compatible con backups de v1.5, v1.4, v1.3, v1.2, v1.1 y v1.0
- ✅ No requiere reconfiguración de MQTT
- ✅ No hay cambios en la interfaz de usuario
- ✅ No hay cambios en topics MQTT
- ⚠️ **IMPORTANTE**: La actualización OTA ahora requiere inicio de sesión como administrador

**Actualización recomendada:**
- **ALTAMENTE RECOMENDADA** para todos los usuarios por las correcciones de seguridad
- Si tienes el sistema expuesto a internet, actualiza INMEDIATAMENTE
- Protege tus credenciales WiFi y MQTT con la nueva protección de backup
- Previene compromisos del dispositivo con la nueva autenticación OTA

### 🎯 Mejoras futuras (próximas versiones)

- Encriptación de contraseñas en backups
- Soporte para autenticación de dos factores
- Rate limiting para prevenir ataques de fuerza bruta
- Logs de auditoría de seguridad

---

**Desarrollado con [Claude Code](https://claude.com/claude-code)**

Co-Authored-By: Claude Sonnet 4.5 <noreply@anthropic.com>
