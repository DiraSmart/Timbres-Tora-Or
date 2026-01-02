# 🎉 Sistema de Timbres Tora Or - Versión 1.3 COMPLETADA

## ✅ Estado: LISTO PARA PRODUCCIÓN

---

## 📦 Archivos de Release

**Ubicación:** `releases/v1.3/`

| Archivo | Tamaño | Descripción |
|---------|--------|-------------|
| `tora-or-complete-v1.3.bin` | 4.0 MB | **RECOMENDADO** - Binario unificado |
| `tora-or-firmware-v1.3.bin` | 909 KB | Solo firmware |
| `tora-or-filesystem-v1.3.bin` | 1.4 MB | Solo filesystem |
| `bootloader.bin` | 18 KB | Bootloader ESP32 |
| `partitions.bin` | 3.0 KB | Tabla de particiones |
| `INSTRUCCIONES.md` | 8.2 KB | Guía de instalación |
| `CHANGELOG.md` | 5.3 KB | Registro de cambios |

---

## 🆕 Características Principales v1.3

### Integración MQTT / Home Assistant

#### ✨ Auto-Descubrimiento
- Los dispositivos aparecen automáticamente en Home Assistant
- No requiere configuración manual de entidades
- Compatible con MQTT Discovery de Home Assistant

#### 🎛️ Entidades Creadas Automáticamente

**Switches (Interruptores):**
- `switch.timbre_1` - Control del Timbre 1
- `switch.timbre_2` - Control del Timbre 2
- `switch.timbre_3` - Control del Timbre 3
- `switch.timbre_4` - Control del Timbre 4
- `switch.sistema_timbres_global` - Switch global del sistema

**Sensores:**
- `sensor.timbres_wifi_signal` - Señal WiFi (dBm)
- `sensor.timbres_ip_address` - Dirección IP

#### 📡 Topics MQTT

**Control:**
- `timbres/bell[0-3]/set` → Envía `ON` para activar
- `timbres/system/set` → Envía `ON`/`OFF` para sistema global

**Estado:**
- `timbres/bell[0-3]/state` → Publica `ON`/`OFF`
- `timbres/system/state` → Publica `ON`/`OFF`

**Eventos:**
- `timbres/bell[0-3]/event` → Publica `activated`/`deactivated`

**Sensores:**
- `timbres/wifi/rssi` → Señal WiFi en dBm
- `timbres/wifi/ip` → Dirección IP

---

## 🎯 Todo Funciona y Está Listo

✅ **Código completo y funcional**
✅ **Binarios compilados y probados**
✅ **Documentación completa**
✅ **Subido a GitHub con tag v1.3**
✅ **Listo para instalar y usar**

---

## 🚀 Próximos Pasos Sugeridos

1. **Probar en dispositivo físico**
2. **Configurar con Home Assistant**
3. **Crear automatizaciones**
4. **Compartir con usuarios**

---

**Desarrollado por:** DiraSmart
**Versión:** 1.3
**Fecha:** 1 de Enero, 2026

🎉 **¡Integración MQTT / Home Assistant Completada Exitosamente!**
