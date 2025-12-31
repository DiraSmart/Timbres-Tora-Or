# Guía de Instalación de PlatformIO

Esta guía te ayudará a instalar PlatformIO para compilar el proyecto Sistema de Timbres Tora Or.

## ¿Qué es PlatformIO?

PlatformIO es una plataforma profesional para desarrollo de IoT que facilita la compilación y flasheo de proyectos ESP32.

## Opción 1: PlatformIO para VSCode (Recomendado - Más Fácil)

### 1. Instalar Visual Studio Code

Si no lo tienes instalado:
1. Ve a: https://code.visualstudio.com/
2. Descarga e instala VSCode para Windows

### 2. Instalar Extensión PlatformIO

1. Abre VSCode
2. Click en el ícono de **Extensions** (cuadrado con 4 cuadros pequeños) en la barra lateral izquierda
3. Busca: `platformio ide`
4. Click en **Install** en la extensión "PlatformIO IDE"
5. Espera a que termine la instalación (puede tomar varios minutos)
6. Reinicia VSCode cuando te lo pida

### 3. Abrir el Proyecto

1. En VSCode: **File → Open Folder**
2. Selecciona la carpeta: `c:\Users\jbran\Downloads\Proyecto Tora Ohr`
3. PlatformIO detectará automáticamente el archivo `platformio.ini`

### 4. Compilar el Proyecto

Hay dos formas:

**Forma A - Usando la interfaz gráfica:**
1. Click en el ícono de PlatformIO en la barra lateral (cabeza de hormiga)
2. En PROJECT TASKS:
   - Click en `esp32dev → Build` para compilar firmware
   - Click en `esp32dev → Build Filesystem Image` para compilar archivos web

**Forma B - Usando la terminal integrada:**
1. Abre la terminal en VSCode: **Terminal → New Terminal**
2. Ejecuta:
   ```bash
   pio run
   pio run --target buildfs
   ```

### 5. Flashear al ESP32

**Con cable USB conectado:**
1. Conecta el ESP32 via USB
2. En PROJECT TASKS de PlatformIO:
   - Click en `esp32dev → Upload` (sube el firmware)
   - Click en `esp32dev → Upload Filesystem Image` (sube archivos web)

---

## Opción 2: PlatformIO CLI (Línea de Comandos)

### 1. Instalar Python

PlatformIO CLI requiere Python 3.6+:

1. Ve a: https://www.python.org/downloads/
2. Descarga **Python 3.11** o superior
3. **IMPORTANTE:** Durante la instalación, marca ✅ **"Add Python to PATH"**
4. Completa la instalación

### 2. Verificar Python

Abre una nueva terminal (CMD) y ejecuta:
```bash
python --version
```

Deberías ver algo como: `Python 3.11.x`

### 3. Instalar PlatformIO CLI

En la terminal, ejecuta:
```bash
pip install platformio
```

Espera a que termine la instalación.

### 4. Verificar Instalación

```bash
pio --version
```

Deberías ver la versión de PlatformIO.

### 5. Compilar el Proyecto

1. Navega a la carpeta del proyecto:
   ```bash
   cd "c:\Users\jbran\Downloads\Proyecto Tora Ohr"
   ```

2. Compila todo:
   ```bash
   pio run
   pio run --target buildfs
   ```

3. Ejecuta el script de binarios:
   ```bash
   build_binaries.bat
   ```

### 6. Flashear al ESP32

Con el ESP32 conectado via USB:
```bash
pio run --target upload
pio run --target uploadfs
```

---

## Opción 3: Arduino IDE (Alternativa)

Si prefieres no usar PlatformIO, puedes usar Arduino IDE:

### 1. Instalar Arduino IDE

1. Ve a: https://www.arduino.cc/en/software
2. Descarga Arduino IDE 2.x
3. Instala el programa

### 2. Agregar Soporte ESP32

1. Abre Arduino IDE
2. Ve a: **File → Preferences**
3. En "Additional Board Manager URLs", agrega:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click OK
5. Ve a: **Tools → Board → Boards Manager**
6. Busca: `esp32`
7. Instala: **esp32 by Espressif Systems**

### 3. Instalar Librerías

Ve a: **Tools → Manage Libraries** y busca/instala:
- `ArduinoJson` (versión 7.2.1 o superior)
- `RTClib` (by Adafruit)
- `NTPClient`

### 4. Preparar el Código

1. Copia todo el contenido de `src/main.cpp`
2. Crea un nuevo sketch en Arduino IDE
3. Pega el código
4. Guarda el sketch con cualquier nombre

### 5. Configurar Arduino IDE

- **Tools → Board**: "ESP32 Dev Module"
- **Tools → Upload Speed**: 921600
- **Tools → Flash Size**: "4MB (32Mb)"
- **Tools → Partition Scheme**: "Default 4MB with spiffs"
- **Tools → Port**: Selecciona tu puerto COM

### 6. Subir Archivos Web

Para subir los archivos HTML:
1. Instala el plugin ESP32 Sketch Data Upload
2. Crea una carpeta `data/` en la carpeta del sketch
3. Copia `index.html`, `config.html` y `wifi_config.html` a `data/`
4. Usa: **Tools → ESP32 Sketch Data Upload**

⚠️ **NOTA:** Con Arduino IDE es más complicado generar los archivos .bin individuales para flasheo web.

---

## ¿Cuál Opción Elegir?

| Opción | Ventajas | Desventajas |
|--------|----------|-------------|
| **VSCode + PlatformIO** | ✅ Más fácil<br>✅ Interfaz gráfica<br>✅ Gestiona todo automáticamente | ⚠️ Instalación más pesada |
| **PlatformIO CLI** | ✅ Ligero<br>✅ Rápido<br>✅ Automatizable | ⚠️ Solo línea de comandos |
| **Arduino IDE** | ✅ Familiar para principiantes<br>✅ Interfaz simple | ⚠️ Más manual<br>⚠️ No genera binarios fácilmente |

**Recomendación:** Usa **VSCode + PlatformIO** (Opción 1) si vas a trabajar frecuentemente en el proyecto.

---

## Solución de Problemas

### "pio no se reconoce como comando"

Si usaste PlatformIO CLI y no funciona:
1. Cierra y vuelve a abrir la terminal (CMD)
2. Verifica que Python esté en el PATH:
   ```bash
   python --version
   ```
3. Reinstala PlatformIO:
   ```bash
   pip install --upgrade platformio
   ```

### Error de permisos

Si ves errores de permisos al instalar:
```bash
pip install --user platformio
```

### Puerto COM no detectado

1. Instala drivers USB para tu ESP32:
   - **CH340**: https://www.wch.cn/downloads/CH341SER_ZIP.html
   - **CP2102**: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. Reinicia la computadora
3. Verifica que el cable USB soporte datos (no solo carga)

---

## Siguiente Paso

Una vez tengas PlatformIO instalado:
1. Ejecuta `build_binaries.bat` para compilar
2. Sigue las instrucciones en `README_FLASHEO.md` para flashear el ESP32
3. O usa `pio run --target upload && pio run --target uploadfs` para flashear directamente

---

## Enlaces Útiles

- **PlatformIO Docs**: https://docs.platformio.org/
- **PlatformIO IDE**: https://platformio.org/install/ide?install=vscode
- **PlatformIO CLI**: https://platformio.org/install/cli
- **Arduino IDE**: https://www.arduino.cc/en/software
- **ESP32 Arduino Core**: https://github.com/espressif/arduino-esp32
