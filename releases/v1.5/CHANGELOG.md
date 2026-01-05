# Changelog - v1.5

## [1.5.0] - 2026-01-04

### ✨ Nueva arquitectura de horarios simplificada

#### Sistema de control mejorado
- **Switches por día de la semana**: Ahora puedes activar/desactivar todos los horarios de un día específico (Lunes, Martes, etc.)
- **Switch maestro global**: Activa o desactiva TODOS los horarios del sistema de una vez
- **Eliminación de switches por timbre**: Se eliminaron los switches individuales por timbre para simplificar la gestión
- **Vista consolidada**: Los horarios que suenan a la misma hora se muestran agrupados en una sola línea con todos los timbres que sonarán

#### Interfaz de usuario mejorada
- **Vista por días de la semana**: Cada día tiene su propia sección con:
  - Icono distintivo para cada día de la semana
  - Contador de horarios configurados
  - Switch maestro para activar/desactivar el día completo
- **Horarios consolidados**:
  - Si varios timbres suenan a la misma hora y tienen el mismo nombre, se muestran en una sola línea
  - Los nombres de los timbres se muestran separados por comas (ej: "Timbre 1, Timbre 2, Timbre 3")
  - Simplifica la visualización cuando hay múltiples timbres sonando al mismo tiempo
- **Feedback visual mejorado**:
  - Los horarios se muestran en gris cuando el día está desactivado
  - Mensaje claro de estado: "Activo", "Inactivo" o "Día desactivado"
  - Colores distintivos para cada estado

#### MQTT / Home Assistant
- **Nuevos topics por día**:
  - `timbres/schedules/day0/set` - Activar/desactivar Lunes (payload: `ON`/`OFF`)
  - `timbres/schedules/day1/set` - Activar/desactivar Martes (payload: `ON`/`OFF`)
  - `timbres/schedules/day2/set` - Activar/desactivar Miércoles (payload: `ON`/`OFF`)
  - `timbres/schedules/day3/set` - Activar/desactivar Jueves (payload: `ON`/`OFF`)
  - `timbres/schedules/day4/set` - Activar/desactivar Viernes (payload: `ON`/`OFF`)
  - `timbres/schedules/day5/set` - Activar/desactivar Sábado (payload: `ON`/`OFF`)
  - `timbres/schedules/day6/set` - Activar/desactivar Domingo (payload: `ON`/`OFF`)
  - `timbres/schedules/set` - Switch maestro global (payload: `ON`/`OFF`)

- **Auto-descubrimiento mejorado**:
  - Nuevos switches en Home Assistant para cada día de la semana
  - Switch maestro global para todo el sistema de horarios
  - Iconos actualizados: `mdi:calendar-week` para días, `mdi:calendar-clock` para switch maestro
  - Se eliminaron los switches por timbre (ya no son necesarios)

- **Sincronización bidireccional**:
  - Cambios desde Home Assistant se reflejan en la interfaz web del ESP32
  - Cambios desde la interfaz web se publican a Home Assistant
  - Actualización automática cada 2 segundos

### 🔧 Cambios técnicos

#### Backend (src/main.cpp)
- Simplificación de `checkSchedules()`: Ahora solo verifica el switch global y el switch del día actual
- Eliminación de `bellSchedulesEnabled[]`: Ya no se necesitan switches por timbre
- Nuevo array `daySchedulesEnabled[]`: Control individual por día de la semana
- Actualizada versión de software de "1.4" a "1.5" en mensajes MQTT
- Nuevos endpoints MQTT discovery para switches de días
- Mejora en la lógica de activación: más eficiente y predecible

#### Frontend (data/index.html)
- Nueva función `renderSchedules()` con consolidación de horarios
- Función `toggleScheduleGroup()` para activar/desactivar grupos de horarios
- Función `deleteScheduleGroup()` para eliminar grupos de horarios
- Función `toggleDayMaster()` para controlar días individuales
- Mejor organización visual por días de la semana
- Reducción de elementos duplicados en la interfaz

#### API REST
- Nuevo endpoint `/api/schedules-switches`:
  - `POST` con `{"global": true/false, "days": [bool, bool, ...]}` para actualizar estados
  - `GET` para obtener estados actuales
- Compatible con la API anterior para `bellSchedulesEnabled` (por retrocompatibilidad en backups)

### 🐛 Correcciones de errores

- **Fix en sincronización MQTT**: Los switches de días se actualizan correctamente desde Home Assistant
- **Mejor manejo de estados**: La desactivación de un día aplica feedback visual inmediato
- **Eliminación correcta de grupos**: Al eliminar un horario consolidado se eliminan todos los timbres asociados

### 📝 Documentación

- Actualizado CHANGELOG.md con todos los cambios de v1.5
- Actualizado INSTRUCCIONES.md con la nueva arquitectura de switches
- Documentados los nuevos topics MQTT
- Agregadas capturas de pantalla de la nueva interfaz

### 🔄 Cambios desde v1.4

**Archivos modificados:**
- `src/main.cpp`: Nueva arquitectura de switches por día
- `data/index.html`: Interfaz consolidada y vista por días
- `data/config.html`: Sin cambios funcionales

**Compatibilidad:**
- ✅ Compatible con backups de v1.4, v1.3, v1.2, v1.1 y v1.0
- ⚠️ Los switches individuales de timbres en Home Assistant se eliminarán automáticamente
- ✅ Se crearán nuevos switches por día de la semana en Home Assistant
- ✅ La configuración MQTT se conserva
- ✅ Los horarios programados se conservan

**Actualización recomendada:**
- Si deseas una gestión más simple de horarios por días de la semana, actualiza a v1.5
- Si usas MQTT/Home Assistant y prefieres controlar días completos en lugar de timbres individuales, actualiza a v1.5
- La interfaz consolidada facilita la visualización cuando múltiples timbres suenan a la misma hora

### ⚠️ Cambios importantes (Breaking Changes)

**Home Assistant / MQTT:**
- Se eliminaron los switches por timbre: `switch.horarios_timbre_1`, `switch.horarios_timbre_2`, etc.
- Se agregaron switches por día: `switch.horarios_lunes`, `switch.horarios_martes`, etc.
- Si tenías automatizaciones que usaban los switches de timbres, necesitarás actualizarlas para usar los nuevos switches de días

**Migración de automatizaciones:**
- Antes: `switch.horarios_timbre_1` → Ahora: Usa el switch del día + el switch maestro
- Ejemplo: Para desactivar todos los horarios del lunes, usa `switch.horarios_lunes`

### 🎯 Próximos pasos (futuras versiones)

- Considerando agregar presets de horarios (ej: "Horario de verano", "Horario de invierno")
- Explorando la posibilidad de clonar horarios de un día a otro
- Mejoras en la visualización de estadísticas de uso

---

**Desarrollado con [Claude Code](https://claude.com/claude-code)**

Co-Authored-By: Claude Sonnet 4.5 <noreply@anthropic.com>
