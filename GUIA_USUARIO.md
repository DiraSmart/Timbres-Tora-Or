# Guía de Usuario - Sistema de Timbres Tora Or

## 📱 Acceso al Sistema

### Primera Vez (Configuración WiFi)
1. El ESP32 creará una red WiFi llamada: **Timbres-Escuela-Config**
2. Contraseña: **12345678**
3. Conéctate a esta red desde tu celular o computadora
4. Automáticamente se abrirá la página de configuración
5. Si no se abre, ve a: http://192.168.4.1
6. Configura tu WiFi local y guarda

### Acceso Normal
Una vez configurado el WiFi, accede al sistema:
- Si conoces la IP: http://[IP-DEL-ESP32]
- Consulta el router para ver qué IP se le asignó al ESP32
- O usa mDNS si está disponible

---

## 🔔 Gestión de Horarios

### Crear un Horario Simple (Un Solo Timbre)

1. Haz clic en el botón **"+ Agregar Horario"**
2. Selecciona **un timbre** haciendo clic en su botón
   - El botón se pondrá azul cuando esté seleccionado
   - Verás: "✓ 1 timbre seleccionado"
3. Selecciona la **hora** (ej: 07:00)
4. Selecciona los **días** (haz clic en L, M, X, J, V, S, D)
   - Los días seleccionados se pondrán azules
5. Haz clic en **"Guardar"**

**Resultado:** Se crea 1 horario para el timbre seleccionado.

---

### ✨ Crear Horarios Múltiples (NUEVA FUNCIONALIDAD)

Si necesitas el mismo horario en varios timbres:

1. Haz clic en **"+ Agregar Horario"**
2. Selecciona **varios timbres** haciendo clic en cada uno
   - Puedes seleccionar 2, 3 o los 4 timbres
   - Verás: "✓ X timbres seleccionados - se creará el mismo horario en todos"
3. Selecciona la **hora** (ej: 12:00)
4. Selecciona los **días** (ej: L, M, X, J, V para lunes a viernes)
5. Haz clic en **"Guardar"**

**Resultado:** Se crean múltiples horarios (uno por cada timbre seleccionado) con la misma hora y días.

**Ejemplo práctico:**
Necesitas que todos los timbres suenen a las 7:00 AM de lunes a viernes:
- Selecciona los 4 timbres ✓
- Hora: 07:00
- Días: L, M, X, J, V
- Guardar

En lugar de crear 4 horarios por separado, lo haces en 1 sola operación.

---

### Editar un Horario

1. Busca el horario en la lista del timbre correspondiente
2. Haz clic en **"Editar"**
3. Modifica la hora o los días
   - **Nota:** En modo edición solo puedes seleccionar un timbre (el del horario)
4. Haz clic en **"Guardar"**

---

### Eliminar un Horario

1. Busca el horario en la lista
2. Haz clic en **"Eliminar"**
3. Confirma la eliminación

---

## 🎛️ Control de Horarios

### Desactivar/Activar un Horario Individual

Cada horario tiene un switch (interruptor):
- **Verde (ON):** El horario está activo y sonará
- **Gris (OFF):** El horario está desactivado temporalmente

Para cambiar:
- Haz clic en el switch del horario
- El cambio se guarda automáticamente

**Uso común:** Desactivar un horario por un día sin eliminarlo.

---

### Desactivar/Activar Todos los Horarios de un Timbre

En la cabecera de cada timbre hay un switch maestro "Todos":
- **Verde (ON):** Todos los horarios de ese timbre están activos
- **Gris (OFF):** Todos los horarios de ese timbre están desactivados

Para cambiar:
- Haz clic en el switch "Todos" del timbre
- Activa o desactiva todos los horarios a la vez

**Uso común:** Desactivar un timbre completo los fines de semana o días festivos.

---

## ⚙️ Configuración

Haz clic en el ícono **⚙️** (arriba a la derecha) para acceder a:

### 1. Nombrar Timbres
- Cambia "Timbre 1", "Timbre 2", etc. por nombres personalizados
- Ejemplo: "Entrada", "Recreo", "Salida", "Emergencia"
- Los nombres se actualizan en toda la interfaz

### 2. Duración de Timbres ⏱️ (NUEVO)
- Configura cuánto tiempo sonará cada timbre
- Rango: 0.1 a 60 segundos
- Puedes usar decimales (ej: 2.5 segundos)
- Cada timbre tiene su propia duración independiente

**Ejemplo:**
- Timbre Entrada: 5 segundos (más largo para que todos escuchen)
- Timbre Recreo: 3 segundos (normal)
- Timbre Salida: 4 segundos
- Timbre Emergencia: 10 segundos (muy largo)

**Cómo configurar:**
1. Ve a Configuración → Duración de Timbres
2. Ingresa la duración deseada para cada timbre
3. Haz clic en "Guardar Duraciones"
4. Prueba el timbre para verificar la duración

### 3. Probar Timbres
- Haz clic en el botón de cada timbre para activarlo manualmente
- Útil para verificar conexiones, volumen y duración configurada
- El timbre sonará con la duración que configuraste

### 4. Sincronizar Hora
- **Sincronizar con Internet (NTP):**
  - Requiere conexión WiFi a internet
  - Sincroniza automáticamente con servidores de hora
  - Ajusta a zona horaria de Panamá (GMT-5)

- **Configurar Manualmente:**
  - Si no hay internet
  - Ingresa fecha y hora actual
  - Guardar

### 5. Configuración WiFi
- Ver estado de conexión WiFi actual
- Cambiar red WiFi
- Ver IP asignada

---

## 📋 Visualización de Horarios

Los horarios se organizan por timbre en la pantalla principal:

```
┌─────────────────────────────────┐
│ Timbre 1          ○ Todos  [5]  │ ← Nombre, toggle maestro, contador
├─────────────────────────────────┤
│ ⏰ 07:00  LMXJV  [ON] [Editar]  │ ← Horario individual
│ ⏰ 12:00  LMXJV  [ON] [Editar]  │
│ ⏰ 14:00  LMXJV  [ON] [Editar]  │
└─────────────────────────────────┘
```

- **Contador:** Muestra cuántos horarios tiene ese timbre
- **Días:** L=Lunes, M=Martes, X=Miércoles, J=Jueves, V=Viernes, S=Sábado, D=Domingo
- **Toggle:** ON (verde) = activo, OFF (gris) = desactivado

---

## 🔧 Solución de Problemas

### Los timbres no suenan
1. ✓ Verifica que el horario esté **activado** (switch verde)
2. ✓ Verifica que el toggle **maestro** del timbre esté activado
3. ✓ Verifica que los **días** incluyan el día actual
4. ✓ Verifica que la **hora del sistema** sea correcta (ve a Configuración)
5. ✓ Prueba el timbre manualmente desde Configuración

### No puedo acceder al sistema
1. ✓ Verifica que estés conectado a la misma red WiFi
2. ✓ Intenta acceder a http://192.168.4.1 (modo AP)
3. ✓ Verifica que el ESP32 esté encendido (LED de power)
4. ✓ Reinicia el ESP32 (botón RESET)

### La hora está incorrecta
1. ✓ Ve a Configuración → Sincronizar Hora
2. ✓ Si hay internet, usa "Sincronizar con Internet"
3. ✓ Si no hay internet, configura manualmente
4. ✓ Verifica la zona horaria (debe ser GMT-5 para Panamá)

### El WiFi no conecta
1. ✓ Verifica que la contraseña sea correcta
2. ✓ Verifica que el router esté encendido
3. ✓ El ESP32 reintentará cada 2 minutos automáticamente
4. ✓ Mientras tanto, puedes acceder en modo AP (192.168.4.1)

---

## 💡 Consejos y Buenas Prácticas

### ✅ Organización de Horarios
- Usa nombres descriptivos para los timbres (Entrada, Recreo, etc.)
- Agrupa horarios similares usando la selección múltiple
- Desactiva temporalmente en lugar de eliminar si es solo por un día

### ✅ Mantenimiento
- Sincroniza la hora con internet semanalmente
- Verifica que la batería del RTC esté cargada
- Prueba los timbres manualmente cada mes

### ✅ Días Festivos
- Desactiva el toggle maestro de cada timbre
- No es necesario eliminar horarios
- Reactiva cuando vuelvan las clases

### ✅ Cambios de Horario
- Durante cambios de horario escolar:
  - Opción 1: Edita los horarios existentes
  - Opción 2: Desactiva los viejos, crea nuevos
  - Opción 3: Elimina y recrea

### ✅ Respaldo
- Toma capturas de pantalla de tus horarios configurados
- Anota la configuración en papel como respaldo
- Considera exportar la configuración (función futura)

---

## 📞 Soporte Técnico

Si necesitas ayuda adicional:
- Consulta el archivo README.md para documentación técnica
- Revisa el archivo CHANGELOG.md para ver nuevas funcionalidades
- Contacta al administrador del sistema

---

## 🎯 Resumen Rápido

| Acción | Pasos |
|--------|-------|
| **Crear horario simple** | Agregar → 1 timbre → hora → días → Guardar |
| **Crear horarios múltiples** | Agregar → varios timbres → hora → días → Guardar |
| **Desactivar horario** | Click en switch del horario |
| **Desactivar timbre completo** | Click en toggle "Todos" |
| **Cambiar nombre timbre** | ⚙️ → Nombrar Timbres → editar → Guardar |
| **Configurar duración timbre** | ⚙️ → Duración de Timbres → ingresar segundos → Guardar |
| **Probar timbre** | ⚙️ → Probar Timbres → click en botón |
| **Sincronizar hora** | ⚙️ → Sincronizar con Internet |
| **Configurar WiFi** | ⚙️ → Configuración WiFi |

---

**¡Disfruta de tu Sistema de Timbres Tora Or! 🔔**
