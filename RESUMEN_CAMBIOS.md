# Resumen de Cambios Realizados

## ✅ Funcionalidad: Duración Configurable por Timbre

### 📋 Descripción
Se agregó la capacidad de configurar cuánto tiempo sonará cada timbre de forma independiente. Cada uno de los 4 timbres puede tener una duración diferente, desde 0.1 segundos hasta 60 segundos.

---

## 🔧 Cambios en el Backend (src/main.cpp)

### 1. **Variable de duraciones**
```cpp
// Antes:
const int BELL_DURATION = 3000;

// Ahora:
int bellDurations[4] = {3000, 3000, 3000, 3000};
```
- Cambió de una constante única a un array de 4 duraciones
- Cada timbre tiene su propia duración configurable

### 2. **Funciones nuevas agregadas**
- `loadBellDurations()` - Carga las duraciones desde LittleFS
- `saveBellDurations()` - Guarda las duraciones en LittleFS
- `handleGetBellDurations()` - API GET para obtener duraciones
- `handleSaveBellDurations()` - API POST para guardar duraciones

### 3. **Función modificada**
```cpp
// Función updateBells() actualizada para usar duraciones individuales
if (bellActive[i] && (currentTime - bellStartTime[i] >= bellDurations[i])) {
    // Ahora usa bellDurations[i] en lugar de BELL_DURATION
}
```

### 4. **Nuevos endpoints de API**
- `GET /api/bell-durations` - Obtiene las duraciones actuales
- `POST /api/bell-durations` - Guarda nuevas duraciones

### 5. **Inicialización**
- `initFileSystem()` ahora llama a `loadBellDurations()`
- Las duraciones se cargan automáticamente al iniciar

### 6. **Almacenamiento**
- Archivo: `/bell_durations.json` en LittleFS
- Formato: `{"durations": [3000, 3000, 3000, 3000]}`
- Valores en milisegundos

### 7. **Validación**
- Duración mínima: 100ms (0.1 segundos)
- Duración máxima: 60000ms (60 segundos)
- Validación en el servidor al guardar

---

## 🎨 Cambios en el Frontend (data/config.html)

### 1. **Nueva sección HTML**
Se agregó la sección "Duración de Timbres" con:
- 4 campos de entrada numérica (uno por timbre)
- Labels dinámicos que usan los nombres personalizados de los timbres
- Rango: 0.1 a 60 segundos
- Incrementos de 0.1 segundos
- Botón "Guardar Duraciones"

### 2. **Funciones JavaScript nuevas**
- `loadBellDurations()` - Carga y muestra las duraciones actuales
- `saveBellDurations()` - Envía las nuevas duraciones al servidor

### 3. **Conversión automática**
- Backend: Milisegundos (1000 = 1 segundo)
- Frontend: Segundos (más fácil para el usuario)
- Conversión automática en ambas direcciones

### 4. **Inicialización**
- `loadBellDurations()` se llama al cargar la página
- Valores por defecto: 3.0 segundos

### 5. **Integración con nombres**
- Las etiquetas de duración se actualizan cuando cambias los nombres de los timbres
- Ejemplo: Si cambias "Timbre 1" a "Entrada", la etiqueta dirá "Entrada"

---

## 📊 Flujo de Datos

```
Usuario ingresa duración en segundos (ej: 5.5)
           ↓
JavaScript convierte a milisegundos (5500)
           ↓
POST /api/bell-durations
           ↓
handleSaveBellDurations() valida (100-60000ms)
           ↓
saveBellDurations() guarda en LittleFS
           ↓
bellDurations[i] se actualiza en RAM
           ↓
updateBells() usa la nueva duración
           ↓
Timbre suena por el tiempo configurado
```

---

## 🎯 Casos de Uso

### Caso 1: Timbres con diferentes propósitos
```
Timbre Entrada:    5 segundos  (más largo, importante)
Timbre Recreo:     3 segundos  (duración estándar)
Timbre Salida:     4 segundos  (medio)
Timbre Emergencia: 10 segundos (muy largo, urgente)
```

### Caso 2: Ajuste según volumen
```
Timbre Edificio A: 2 segundos (bocina potente)
Timbre Edificio B: 5 segundos (bocina débil)
Timbre Patio:      4 segundos (área grande)
Timbre Oficina:    1 segundo  (área pequeña)
```

### Caso 3: Diferentes tipos de señal
```
Timbre Clase:      3 segundos   (normal)
Timbre Recreo:     5 segundos   (más largo)
Timbre Simulacro:  0.5 segundos (pulsos cortos)
Timbre Alarma:     15 segundos  (continuo)
```

---

## 🔒 Seguridad y Validación

### Backend
- ✅ Validación de rango: 100-60000ms
- ✅ Valores fuera de rango se ignoran
- ✅ Mantiene valores antiguos si la validación falla
- ✅ Manejo de errores en lectura/escritura de archivos

### Frontend
- ✅ Campos numéricos con atributos HTML5 min/max
- ✅ Step de 0.1 para precisión decimal
- ✅ Campo requerido (no puede estar vacío)
- ✅ Notificaciones de éxito/error

---

## 📁 Archivos Modificados

### Backend
1. **src/main.cpp**
   - Línea 24: Variable `bellDurations[4]`
   - Línea 289-330: Funciones load/save
   - Línea 425: Uso de duraciones individuales en `updateBells()`
   - Línea 523-561: Handlers de API
   - Línea 796-797: Registro de endpoints
   - Línea 243: Carga en `initFileSystem()`

### Frontend
2. **data/config.html**
   - Línea 306-341: Sección HTML de duraciones
   - Línea 420: Llamada a `loadBellDurations()` en init
   - Línea 439: Actualización de labels en `loadBellNames()`
   - Línea 469: Actualización de labels en `saveBellNames()`
   - Línea 481-498: Función `loadBellDurations()`
   - Línea 500-529: Función `saveBellDurations()`

### Documentación
3. **CHANGELOG.md** - Agregada nueva funcionalidad
4. **GUIA_USUARIO.md** - Actualizada sección de configuración
5. **RESUMEN_CAMBIOS.md** - Este archivo (documentación técnica)

---

## 🧪 Cómo Probar

### Prueba 1: Configuración básica
1. Accede a Configuración (⚙️)
2. Verifica que los campos muestren "3.0" (valor por defecto)
3. Cambia Timbre 1 a "5.0" segundos
4. Guarda
5. Verifica mensaje de éxito
6. Recarga la página
7. Verifica que el valor se mantuvo en "5.0"

### Prueba 2: Prueba de timbre
1. Configura duraciones diferentes para cada timbre
2. Ve a "Probar Timbres"
3. Activa cada timbre uno por uno
4. Verifica que suenen por el tiempo configurado

### Prueba 3: Validación
1. Intenta ingresar 0.05 segundos (menos del mínimo)
2. HTML5 no debe permitirlo
3. Intenta ingresar 100 segundos (más del máximo)
4. HTML5 no debe permitirlo

### Prueba 4: Integración con nombres
1. Cambia el nombre de "Timbre 1" a "Entrada"
2. Verifica que en "Duración de Timbres" diga "Entrada"
3. Los labels se actualizan automáticamente

### Prueba 5: Persistencia
1. Configura duraciones personalizadas
2. Reinicia el ESP32 (botón RESET)
3. Vuelve a acceder a Configuración
4. Verifica que las duraciones se mantienen

---

## 🐛 Posibles Problemas y Soluciones

### Problema: Las duraciones no se guardan
**Causa:** LittleFS no está montado correctamente
**Solución:** Verifica el serial monitor, debe decir "Sistema de archivos inicializado"

### Problema: Los timbres no respetan la duración configurada
**Causa:** Archivo de duraciones corrupto
**Solución:** Borra `/bell_durations.json` y configura de nuevo

### Problema: No se pueden ingresar decimales
**Causa:** Teclado del sistema
**Solución:** Usa punto (.) no coma (,) para decimales

### Problema: Duración muy corta (no suena)
**Causa:** Valor menor a 0.1 segundos
**Solución:** Mínimo 0.1 segundos (100ms)

---

## 📈 Mejoras Futuras Posibles

1. **Diferentes duraciones por horario**
   - Permitir que un mismo timbre tenga duraciones diferentes según el horario
   - Ejemplo: 3 segundos en la mañana, 5 segundos al mediodía

2. **Patrones de timbre**
   - Timbres intermitentes (ej: 2 pulsos de 1 segundo con pausa)
   - Timbre progresivo (aumenta de volumen)

3. **Presets de duración**
   - "Rápido" (1s), "Normal" (3s), "Largo" (5s), "Alarma" (10s)
   - Aplicar preset a todos los timbres a la vez

4. **Vista previa de duración**
   - Barra de progreso que muestre visualmente la duración
   - Mientras configuras, ver cuánto tiempo es

5. **Historial de activaciones**
   - Registrar cuántas veces sonó cada timbre
   - Tiempo total de uso

---

## ✅ Checklist de Implementación

- [x] Backend: Variable de duraciones
- [x] Backend: Funciones load/save
- [x] Backend: Endpoints de API
- [x] Backend: Validación de datos
- [x] Backend: Uso de duraciones en updateBells()
- [x] Backend: Inicialización en setup
- [x] Frontend: Sección HTML
- [x] Frontend: Funciones JavaScript
- [x] Frontend: Conversión segundos/milisegundos
- [x] Frontend: Integración con nombres
- [x] Frontend: Notificaciones
- [x] Documentación: CHANGELOG.md
- [x] Documentación: GUIA_USUARIO.md
- [x] Documentación: RESUMEN_CAMBIOS.md

---

## 📝 Notas Técnicas

### Por qué milisegundos en backend
- `millis()` en Arduino devuelve milisegundos
- Más precisión para comparaciones
- Estándar en programación de microcontroladores

### Por qué segundos en frontend
- Más intuitivo para usuarios
- Facilita el ingreso de valores comunes (3, 5, 10)
- Los decimales permiten precisión cuando se necesita

### Estructura de datos
```json
// Formato en LittleFS
{
  "durations": [3000, 5000, 4000, 10000]
}

// Formato en API (igual)
{
  "durations": [3000, 5000, 4000, 10000]
}
```

### Límites elegidos
- **Mínimo (0.1s):** Suficiente para un "click" audible
- **Máximo (60s):** Suficiente para alarmas prolongadas
- Balance entre usabilidad y seguridad

---

**Implementado por:** Sistema de Timbres Tora Or
**Fecha:** 2024
**Versión:** Compatible con todas las versiones del proyecto
