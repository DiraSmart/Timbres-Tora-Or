# Historial de Cambios - Sistema de Timbres Tora Or

## [Última Actualización] - 2024

### ✨ Nueva Funcionalidad: Duración Configurable por Timbre

**Descripción:**
Ahora puedes configurar cuánto tiempo sonará cada timbre de forma independiente desde la página de configuración.

**Cómo funciona:**
1. Ve a Configuración (ícono ⚙️)
2. En la sección "Duración de Timbres", verás 4 campos (uno por timbre)
3. Configura la duración en segundos (de 0.1 a 60 segundos)
4. Haz clic en "Guardar Duraciones"

**Características:**
- ⏱️ Duración independiente para cada timbre (de 0.1 a 60 segundos)
- 🎯 Configuración en segundos (con decimales)
- 💾 Se guarda automáticamente en la memoria del ESP32
- 🔄 Valor por defecto: 3 segundos

**Ejemplo de uso:**
- Timbre 1 (Entrada): 5 segundos
- Timbre 2 (Recreo): 3 segundos
- Timbre 3 (Salida): 4 segundos
- Timbre 4 (Emergencia): 10 segundos

---

### ✨ Nueva Funcionalidad: Selección Múltiple de Timbres

**Descripción:**
Ahora al crear un nuevo horario, puedes seleccionar múltiples timbres a la vez. Esto te permite crear el mismo horario para varios timbres en una sola operación.

**Cómo funciona:**
1. Al hacer clic en "Agregar Horario", verás 4 botones con los nombres de los timbres
2. Puedes hacer clic en uno o varios timbres para seleccionarlos (se pondrán azules)
3. Configura la hora y los días como siempre
4. Al guardar, se creará un horario independiente para cada timbre seleccionado

**Beneficios:**
- ⚡ Más rápido para configurar horarios iguales en múltiples timbres
- 🎯 Menos clicks y menos repetición
- 📊 Feedback visual en tiempo real de cuántos timbres has seleccionado

**Ejemplo de uso:**
Si necesitas que todos los timbres suenen a las 7:00 AM de lunes a viernes:
1. Click en "Agregar Horario"
2. Selecciona los 4 timbres (Timbre 1, 2, 3 y 4)
3. Configura hora: 7:00
4. Selecciona días: L, M, X, J, V
5. Guardar

Resultado: 4 horarios creados (uno por timbre) en una sola operación.

**Notas importantes:**
- ✅ En modo "Agregar": Puedes seleccionar múltiples timbres
- ⚠️ En modo "Editar": Solo puedes editar el timbre del horario existente (no se permite cambiar a múltiples)

---

## Cambios Anteriores

### Cambio de Nombre
- Actualizado el título del sistema de "Sistema de Timbres Escolares" a "Sistema de Timbres Tora Or"
- Archivos actualizados:
  - index.html
  - config.html
  - wifi_config.html
  - main.cpp
  - README.md
  - Todos los archivos de documentación

### Mejoras en Compilación
- Agregado script PowerShell (`build_binaries.ps1`) para compilación
- Mejorado script batch (`build_binaries.bat`) con detección de PlatformIO
- Creada guía completa de instalación de PlatformIO
- Agregada documentación de cómo compilar según el tipo de terminal

### Funcionalidad Base
- ✅ Control de 4 relés independientes
- ✅ RTC DS3231 para funcionamiento sin internet
- ✅ Sincronización NTP cuando hay internet
- ✅ Captive Portal WiFi en 192.168.4.1
- ✅ Reconexión automática WiFi cada 2 minutos
- ✅ Interfaz web responsive
- ✅ Nombres personalizables para cada timbre
- ✅ Prueba manual de timbres
- ✅ Activación/desactivación individual de horarios
- ✅ Toggle maestro por timbre
- ✅ Configuración de días de la semana
- ✅ Zona horaria de Panamá (GMT-5)

---

## Roadmap Futuro (Sugerencias)

Posibles mejoras para futuras versiones:

- [ ] Copiar horarios de un timbre a otro
- [ ] Importar/exportar configuración completa
- [ ] Horarios especiales (días festivos)
- [ ] Diferentes duraciones de timbre por horario
- [ ] Previsualización de horarios del día
- [ ] Logs de activación de timbres
- [ ] Modo silencioso temporal
- [ ] Notificaciones móviles cuando un timbre suena
- [ ] Modo de prueba (sin activar relés reales)

---

## Soporte

Para reportar problemas o sugerir mejoras, contacta al administrador del sistema.
