#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Update.h>
#include <PubSubClient.h>

// Configuración Access Point
const char* ap_ssid = "Timbres-Escuela-Config";
const char* ap_password = "12345678"; // Mínimo 8 caracteres
const IPAddress ap_ip(192, 168, 4, 1);
const IPAddress ap_gateway(192, 168, 4, 1);
const IPAddress ap_subnet(255, 255, 255, 0);

// Pines de los relés (ajusta según tu configuración)
const int RELAY_PINS[4] = {32, 33, 25, 26};
String bellNames[4] = {"Timbre 1", "Timbre 2", "Timbre 3", "Timbre 4"};

// Duración de cada timbre en milisegundos (por defecto 3 segundos cada uno)
int bellDurations[4] = {3000, 3000, 3000, 3000};

// Visibilidad de timbres (por defecto todos visibles)
bool bellVisibility[4] = {true, true, true, true};

// Timeouts y configuración
const unsigned long WIFI_CONNECT_TIMEOUT = 10000; // 10 segundos intentando conectar
const unsigned long WIFI_RETRY_INTERVAL = 120000; // 2 minutos entre reintentos
const unsigned long WIFI_CHECK_INTERVAL = 5000;   // 5 segundos para verificar conexión

// RTC
RTC_DS3231 rtc;
bool rtcAvailable = false;

// Sistema de tiempo alternativo (cuando no hay RTC)
unsigned long ntpEpochTime = 0;  // Tiempo epoch desde NTP
unsigned long ntpLastUpdate = 0; // Última actualización con millis()
bool ntpTimeValid = false;       // Si tenemos hora válida de NTP

// NTP Client (UTC-5 para Panamá = -18000 segundos)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000);

// Web Server y DNS Server
WebServer server(80);
DNSServer dnsServer;

// ============== MQTT Configuration ==============
String mqtt_server = "";
int mqtt_port = 1883;
String mqtt_user = "";
String mqtt_password = "";
String mqtt_client_id = "timbres-tora-or";
bool mqtt_enabled = false;
bool mqtt_ha_discovery = true;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
unsigned long lastMqttReconnectAttempt = 0;
const unsigned long MQTT_RECONNECT_INTERVAL = 5000; // 5 segundos

// MQTT Topics
const String MQTT_BASE_TOPIC = "timbres";
bool globalSystemEnabled = true; // Switch global para activar/desactivar sistema

// Forward declarations
void activateBell(int bellIndex);

// Variables de estado WiFi
enum WiFiMode { MODE_AP, MODE_STA, MODE_BOTH };
WiFiMode currentMode = MODE_AP;
String saved_ssid = "";
String saved_password = "";
unsigned long lastWiFiAttempt = 0;
unsigned long lastWiFiCheck = 0;
bool wifiConfigured = false;

// Estructura para almacenar horarios
struct Schedule {
  char name[31]; // Nombre del horario (máx 30 caracteres + \0)
  bool enabled;
  int hour;
  int minute;
  bool days[7]; // Lunes=0, Domingo=6
  int bellIndex;
};

// Máximo 20 horarios por timbre (80 total)
const int MAX_SCHEDULES = 80;
Schedule schedules[MAX_SCHEDULES];
int scheduleCount = 0;

// Control de timbre activo
unsigned long bellStartTime[4] = {0, 0, 0, 0};
bool bellActive[4] = {false, false, false, false};

// Última verificación de horarios
int lastCheckMinute = -1;

// Sistema de autenticación
const char* ADMIN_USER = "dirasmart";
const char* ADMIN_PASS = "dirasmart1";
String normalUser = "";
String normalPass = "";
bool normalUserConfigured = false;

// Sesiones activas (simple token)
String activeSession = "";
unsigned long sessionExpiry = 0;
bool activeSessionIsAdmin = false;
const unsigned long SESSION_DURATION = 3600000; // 1 hora

// ============== FUNCIONES DE GESTIÓN WiFi ==============

void loadWiFiConfig() {
  File file = LittleFS.open("/wifi.json", "r");
  if (!file) {
    Serial.println("No hay configuración WiFi guardada");
    wifiConfigured = false;
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo configuración WiFi");
    wifiConfigured = false;
    return;
  }

  saved_ssid = doc["ssid"].as<String>();
  saved_password = doc["password"].as<String>();
  wifiConfigured = (saved_ssid.length() > 0);

  if (wifiConfigured) {
    Serial.printf("Configuración WiFi cargada: %s\n", saved_ssid.c_str());
  }
}

void saveWiFiConfig(const String& ssid, const String& password) {
  JsonDocument doc;
  doc["ssid"] = ssid;
  doc["password"] = password;

  File file = LittleFS.open("/wifi.json", "w");
  if (!file) {
    Serial.println("Error guardando configuración WiFi");
    return;
  }

  serializeJson(doc, file);
  file.close();

  saved_ssid = ssid;
  saved_password = password;
  wifiConfigured = true;

  Serial.printf("Configuración WiFi guardada: %s\n", ssid.c_str());
}

// ============== FUNCIONES MQTT ==============

void loadMQTTConfig() {
  File file = LittleFS.open("/mqtt.json", "r");
  if (!file) {
    Serial.println("No hay configuración MQTT guardada");
    mqtt_enabled = false;
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo configuración MQTT");
    mqtt_enabled = false;
    return;
  }

  mqtt_server = doc["server"].as<String>();
  mqtt_port = doc["port"] | 1883;
  mqtt_user = doc["user"].as<String>();
  mqtt_password = doc["password"].as<String>();
  mqtt_client_id = doc["client_id"] | "timbres-tora-or";
  mqtt_enabled = doc["enabled"] | false;
  mqtt_ha_discovery = doc["ha_discovery"] | true;

  if (mqtt_enabled && mqtt_server.length() > 0) {
    Serial.printf("Configuración MQTT cargada: %s:%d\n", mqtt_server.c_str(), mqtt_port);
    mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
  }
}

void saveMQTTConfig() {
  JsonDocument doc;
  doc["server"] = mqtt_server;
  doc["port"] = mqtt_port;
  doc["user"] = mqtt_user;
  doc["password"] = mqtt_password;
  doc["client_id"] = mqtt_client_id;
  doc["enabled"] = mqtt_enabled;
  doc["ha_discovery"] = mqtt_ha_discovery;

  File file = LittleFS.open("/mqtt.json", "w");
  if (!file) {
    Serial.println("Error guardando configuración MQTT");
    return;
  }

  serializeJson(doc, file);
  file.close();

  Serial.printf("Configuración MQTT guardada: %s:%d\n", mqtt_server.c_str(), mqtt_port);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.printf("MQTT Message: %s -> %s\n", topic, message.c_str());

  String topicStr = String(topic);

  // Control de timbres individuales
  for (int i = 0; i < 4; i++) {
    String bellTopic = MQTT_BASE_TOPIC + "/bell" + String(i) + "/set";
    if (topicStr == bellTopic) {
      if (message == "ON" && globalSystemEnabled) {
        activateBell(i);
        mqttClient.publish((MQTT_BASE_TOPIC + "/bell" + String(i) + "/state").c_str(), "ON", true);
      }
      return;
    }
  }

  // Switch global del sistema
  if (topicStr == MQTT_BASE_TOPIC + "/system/set") {
    globalSystemEnabled = (message == "ON");
    mqttClient.publish((MQTT_BASE_TOPIC + "/system/state").c_str(), globalSystemEnabled ? "ON" : "OFF", true);
    Serial.printf("Sistema global: %s\n", globalSystemEnabled ? "ACTIVADO" : "DESACTIVADO");
    return;
  }
}

void publishMQTTDiscovery() {
  if (!mqtt_ha_discovery || !mqttClient.connected()) {
    return;
  }

  Serial.println("Publicando auto-descubrimiento Home Assistant...");

  JsonDocument doc;
  String discoveryTopic;
  String payload;

  // Descubrimiento para cada timbre (switches)
  for (int i = 0; i < 4; i++) {
    doc.clear();
    discoveryTopic = "homeassistant/switch/timbres/bell" + String(i) + "/config";

    doc["name"] = bellNames[i];
    doc["unique_id"] = "timbres_bell_" + String(i);
    doc["command_topic"] = MQTT_BASE_TOPIC + "/bell" + String(i) + "/set";
    doc["state_topic"] = MQTT_BASE_TOPIC + "/bell" + String(i) + "/state";
    doc["icon"] = "mdi:bell-ring";
    doc["device"]["identifiers"][0] = "timbres_tora_or";
    doc["device"]["name"] = "Sistema Timbres Tora Or";
    doc["device"]["manufacturer"] = "DiraSmart";
    doc["device"]["model"] = "ESP32 Bell Controller";
    doc["device"]["sw_version"] = "1.3";

    serializeJson(doc, payload);
    mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
    payload = "";
    delay(100);
  }

  // Switch global del sistema
  doc.clear();
  discoveryTopic = "homeassistant/switch/timbres/system/config";

  doc["name"] = "Sistema Timbres Global";
  doc["unique_id"] = "timbres_system_switch";
  doc["command_topic"] = MQTT_BASE_TOPIC + "/system/set";
  doc["state_topic"] = MQTT_BASE_TOPIC + "/system/state";
  doc["icon"] = "mdi:power";
  doc["device"]["identifiers"][0] = "timbres_tora_or";
  doc["device"]["name"] = "Sistema Timbres Tora Or";
  doc["device"]["manufacturer"] = "DiraSmart";
  doc["device"]["model"] = "ESP32 Bell Controller";
  doc["device"]["sw_version"] = "1.3";

  serializeJson(doc, payload);
  mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
  payload = "";

  // Sensor de WiFi
  doc.clear();
  discoveryTopic = "homeassistant/sensor/timbres/wifi_signal/config";

  doc["name"] = "Timbres WiFi Signal";
  doc["unique_id"] = "timbres_wifi_signal";
  doc["state_topic"] = MQTT_BASE_TOPIC + "/wifi/rssi";
  doc["unit_of_measurement"] = "dBm";
  doc["icon"] = "mdi:wifi";
  doc["device"]["identifiers"][0] = "timbres_tora_or";

  serializeJson(doc, payload);
  mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
  payload = "";

  // Sensor de IP
  doc.clear();
  discoveryTopic = "homeassistant/sensor/timbres/ip_address/config";

  doc["name"] = "Timbres IP Address";
  doc["unique_id"] = "timbres_ip_address";
  doc["state_topic"] = MQTT_BASE_TOPIC + "/wifi/ip";
  doc["icon"] = "mdi:ip-network";
  doc["device"]["identifiers"][0] = "timbres_tora_or";

  serializeJson(doc, payload);
  mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);

  Serial.println("Auto-descubrimiento publicado");
}

boolean reconnectMQTT() {
  if (!mqtt_enabled || mqtt_server.length() == 0) {
    return false;
  }

  Serial.print("Intentando conexión MQTT...");

  boolean connected = false;
  if (mqtt_user.length() > 0) {
    connected = mqttClient.connect(mqtt_client_id.c_str(), mqtt_user.c_str(), mqtt_password.c_str());
  } else {
    connected = mqttClient.connect(mqtt_client_id.c_str());
  }

  if (connected) {
    Serial.println(" conectado!");

    // Suscribirse a topics de control
    for (int i = 0; i < 4; i++) {
      String topic = MQTT_BASE_TOPIC + "/bell" + String(i) + "/set";
      mqttClient.subscribe(topic.c_str());
    }
    mqttClient.subscribe((MQTT_BASE_TOPIC + "/system/set").c_str());

    // Publicar auto-descubrimiento
    publishMQTTDiscovery();

    // Publicar estado inicial
    mqttClient.publish((MQTT_BASE_TOPIC + "/system/state").c_str(), globalSystemEnabled ? "ON" : "OFF", true);

    return true;
  } else {
    Serial.print(" falló, rc=");
    Serial.println(mqttClient.state());
    return false;
  }
}

void publishMQTTSensors() {
  if (!mqttClient.connected()) {
    return;
  }

  // Publicar señal WiFi
  if (WiFi.status() == WL_CONNECTED) {
    int rssi = WiFi.RSSI();
    mqttClient.publish((MQTT_BASE_TOPIC + "/wifi/rssi").c_str(), String(rssi).c_str(), true);
    mqttClient.publish((MQTT_BASE_TOPIC + "/wifi/ip").c_str(), WiFi.localIP().toString().c_str(), true);
  }
}

void publishBellEvent(int bellIndex, const char* event) {
  if (!mqttClient.connected()) {
    return;
  }

  String topic = MQTT_BASE_TOPIC + "/bell" + String(bellIndex) + "/event";
  mqttClient.publish(topic.c_str(), event);

  // Actualizar estado
  if (String(event) == "activated") {
    mqttClient.publish((MQTT_BASE_TOPIC + "/bell" + String(bellIndex) + "/state").c_str(), "ON", true);
  } else if (String(event) == "deactivated") {
    mqttClient.publish((MQTT_BASE_TOPIC + "/bell" + String(bellIndex) + "/state").c_str(), "OFF", true);
  }
}

void handleMQTTConnection() {
  if (!mqtt_enabled || mqtt_server.length() == 0) {
    return;
  }

  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastMqttReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
      lastMqttReconnectAttempt = now;
      if (reconnectMQTT()) {
        lastMqttReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }
}

void startAccessPoint() {
  Serial.println("Iniciando Access Point...");

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(ap_ip, ap_gateway, ap_subnet);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.printf("AP iniciado: %s\n", ap_ssid);
  Serial.printf("IP del AP: %s\n", WiFi.softAPIP().toString().c_str());
  Serial.println("Contraseña: " + String(ap_password));

  // Iniciar servidor DNS para captive portal
  dnsServer.start(53, "*", ap_ip);

  currentMode = MODE_AP;
}

void stopAccessPoint() {
  Serial.println("Deteniendo Access Point...");
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  Serial.println("Access Point detenido");
}

bool connectToWiFi() {
  if (!wifiConfigured || saved_ssid.length() == 0) {
    return false;
  }

  Serial.printf("Intentando conectar a: %s\n", saved_ssid.c_str());

  // Asegurarse de que WiFi esté en modo correcto
  if (currentMode == MODE_AP) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin(saved_ssid.c_str(), saved_password.c_str());

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < WIFI_CONNECT_TIMEOUT) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    currentMode = MODE_STA;

    // Sincronizar con NTP (siempre, incluso sin RTC)
    timeClient.begin();
    if (timeClient.update()) {
      unsigned long epochTime = timeClient.getEpochTime();

      // Actualizar RTC si está disponible
      if (rtcAvailable) {
        rtc.adjust(DateTime(epochTime));
        Serial.println("RTC sincronizado con NTP");
      } else {
        // Si no hay RTC, usar tiempo NTP en memoria
        ntpEpochTime = epochTime;
        ntpLastUpdate = millis();
        ntpTimeValid = true;
        Serial.println("Tiempo sincronizado con NTP (sin RTC)");
      }
    }

    // Apagar el Access Point cuando se conecta exitosamente
    stopAccessPoint();

    // Cambiar a modo solo STA después de apagar el AP
    WiFi.mode(WIFI_STA);

    return true;
  }

  Serial.println("No se pudo conectar");
  return false;
}

void checkWiFiConnection() {
  unsigned long currentMillis = millis();

  // Si estamos conectados, verificar que siga conectado
  if (currentMode == MODE_STA) {
    if (currentMillis - lastWiFiCheck > WIFI_CHECK_INTERVAL) {
      lastWiFiCheck = currentMillis;

      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, iniciando AP...");
        startAccessPoint();
        lastWiFiAttempt = currentMillis;
      }
    }
  }

  // Si estamos en modo AP, intentar reconectar cada 2 minutos
  if (currentMode == MODE_AP && wifiConfigured) {
    if (currentMillis - lastWiFiAttempt > WIFI_RETRY_INTERVAL) {
      Serial.println("Reintentando conexión WiFi...");
      lastWiFiAttempt = currentMillis;

      if (connectToWiFi()) {
        Serial.println("Reconexión exitosa!");
      } else {
        Serial.println("Reconexión fallida, manteniendo AP");
      }
    }
  }
}

// ============== FUNCIONES BÁSICAS ==============

// Forward declarations
void loadBellNames();
void loadBellDurations();
void loadSchedules();
void loadNormalUser();
void saveNormalUser();
void loadBellVisibility();
void saveBellVisibility();

// Funciones de autenticación
String generateToken() {
  String token = "";
  for (int i = 0; i < 32; i++) {
    token += String(random(0, 16), HEX);
  }
  return token;
}

bool checkAuth() {
  Serial.println("[DEBUG] checkAuth() iniciado");

  if (!server.hasHeader("Authorization")) {
    Serial.println("[DEBUG] No hay header Authorization");
    return false;
  }

  String auth = server.header("Authorization");
  Serial.printf("[DEBUG] Token recibido: %s\n", auth.c_str());
  Serial.printf("[DEBUG] Token esperado: %s\n", activeSession.c_str());
  Serial.printf("[DEBUG] Sesión expira en: %lu ms\n", sessionExpiry > millis() ? sessionExpiry - millis() : 0);

  if (auth != activeSession) {
    Serial.println("[DEBUG] Token no coincide");
    return false;
  }

  if (millis() > sessionExpiry) {
    Serial.println("[DEBUG] Sesión expirada");
    return false;
  }

  // Renovar sesión
  sessionExpiry = millis() + SESSION_DURATION;
  Serial.println("[DEBUG] Autenticación exitosa");
  return true;
}

// Verificar si la sesión activa es de administrador
bool checkAdminAuth() {
  if (!checkAuth()) {
    return false;
  }
  return activeSessionIsAdmin;
}

// Función para obtener la hora actual desde RTC o NTP
DateTime getCurrentTime() {
  if (rtcAvailable) {
    return rtc.now();
  } else if (ntpTimeValid) {
    // Calcular tiempo actual basado en NTP + millis()
    unsigned long elapsedSeconds = (millis() - ntpLastUpdate) / 1000;
    unsigned long currentEpoch = ntpEpochTime + elapsedSeconds;
    return DateTime(currentEpoch);
  }
  // Si no hay ninguna fuente, devolver tiempo por defecto
  return DateTime(2024, 1, 1, 0, 0, 0);
}

// Función para verificar si tenemos una fuente de tiempo válida
bool hasValidTime() {
  return rtcAvailable || ntpTimeValid;
}

void initRelays() {
  for (int i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  Serial.println("Relés inicializados");
}

void initRTC() {
  if (!rtc.begin()) {
    Serial.println("ADVERTENCIA: No se encontró el RTC - continuando sin él");
    Serial.println("Los horarios programados NO funcionarán hasta conectar el RTC");
    rtcAvailable = false;
    return; // Continuar sin RTC en lugar de bloquearse
  }

  if (rtc.lostPower()) {
    Serial.println("RTC perdió energía, configurando fecha/hora por defecto");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtcAvailable = true;
  Serial.println("RTC inicializado correctamente");
}

void initFileSystem() {
  if (!LittleFS.begin(true)) {
    Serial.println("Error montando LittleFS");
    return;
  }
  Serial.println("Sistema de archivos inicializado");
  loadWiFiConfig();
  loadMQTTConfig();
  loadBellNames();
  loadBellDurations();
  loadBellVisibility();
  loadSchedules();
  loadNormalUser();
}

void loadBellNames() {
  File file = LittleFS.open("/bell_names.json", "r");
  if (!file) {
    Serial.println("No hay nombres personalizados, usando por defecto");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo nombres de timbres");
    return;
  }

  JsonArray array = doc["names"].as<JsonArray>();
  for (int i = 0; i < 4 && i < array.size(); i++) {
    bellNames[i] = array[i].as<String>();
  }

  Serial.println("Nombres de timbres cargados");
}

void saveBellNames() {
  JsonDocument doc;
  JsonArray array = doc["names"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellNames[i]);
  }

  File file = LittleFS.open("/bell_names.json", "w");
  if (!file) {
    Serial.println("Error guardando nombres de timbres");
    return;
  }

  serializeJson(doc, file);
  file.close();
  Serial.println("Nombres de timbres guardados");
}

void loadBellDurations() {
  File file = LittleFS.open("/bell_durations.json", "r");
  if (!file) {
    Serial.println("No hay duraciones personalizadas, usando por defecto (3000ms)");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo duraciones de timbres");
    return;
  }

  JsonArray array = doc["durations"].as<JsonArray>();
  for (int i = 0; i < 4 && i < array.size(); i++) {
    bellDurations[i] = array[i].as<int>();
  }

  Serial.println("Duraciones de timbres cargadas");
}

void saveBellDurations() {
  JsonDocument doc;
  JsonArray array = doc["durations"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellDurations[i]);
  }

  File file = LittleFS.open("/bell_durations.json", "w");
  if (!file) {
    Serial.println("Error guardando duraciones de timbres");
    return;
  }

  serializeJson(doc, file);
  file.close();
  Serial.println("Duraciones de timbres guardadas");
}

void loadBellVisibility() {
  File file = LittleFS.open("/bell_visibility.json", "r");
  if (!file) {
    Serial.println("No hay visibilidad personalizada, usando por defecto (todos visibles)");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo visibilidad de timbres");
    return;
  }

  JsonArray array = doc["visibility"].as<JsonArray>();
  for (int i = 0; i < 4 && i < array.size(); i++) {
    bellVisibility[i] = array[i].as<bool>();
  }

  Serial.println("Visibilidad de timbres cargada");
}

void saveBellVisibility() {
  JsonDocument doc;
  JsonArray array = doc["visibility"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellVisibility[i]);
  }

  File file = LittleFS.open("/bell_visibility.json", "w");
  if (!file) {
    Serial.println("Error guardando visibilidad de timbres");
    return;
  }

  serializeJson(doc, file);
  file.close();
  Serial.println("Visibilidad de timbres guardada");
}

void initWiFi() {
  // Intentar conectar a WiFi guardado
  if (wifiConfigured && connectToWiFi()) {
    Serial.println("Conectado a WiFi guardado");
    lastWiFiAttempt = millis();
  } else {
    // Si no hay WiFi configurado o falla, iniciar AP
    startAccessPoint();
    lastWiFiAttempt = millis();
  }
}

void loadSchedules() {
  File file = LittleFS.open("/schedules.json", "r");
  if (!file) {
    Serial.println("No hay archivo de horarios, iniciando vacío");
    scheduleCount = 0;
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo horarios");
    scheduleCount = 0;
    return;
  }

  JsonArray array = doc.as<JsonArray>();
  scheduleCount = 0;

  for (JsonObject obj : array) {
    if (scheduleCount >= MAX_SCHEDULES) break;

    // Cargar nombre del horario
    String scheduleName = obj["name"] | "";
    strncpy(schedules[scheduleCount].name, scheduleName.c_str(), sizeof(schedules[scheduleCount].name) - 1);
    schedules[scheduleCount].name[sizeof(schedules[scheduleCount].name) - 1] = '\0';

    schedules[scheduleCount].enabled = obj["enabled"] | true;
    schedules[scheduleCount].hour = obj["hour"];
    schedules[scheduleCount].minute = obj["minute"];
    schedules[scheduleCount].bellIndex = obj["bell"];

    JsonArray daysArray = obj["days"];
    for (int i = 0; i < 7; i++) {
      schedules[scheduleCount].days[i] = daysArray[i];
    }

    scheduleCount++;
  }

  Serial.printf("Cargados %d horarios\n", scheduleCount);
}

void saveSchedules() {
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < scheduleCount; i++) {
    JsonObject obj = array.add<JsonObject>();
    obj["name"] = schedules[i].name;
    obj["enabled"] = schedules[i].enabled;
    obj["hour"] = schedules[i].hour;
    obj["minute"] = schedules[i].minute;
    obj["bell"] = schedules[i].bellIndex;

    JsonArray daysArray = obj["days"].to<JsonArray>();
    for (int j = 0; j < 7; j++) {
      daysArray.add(schedules[i].days[j]);
    }
  }

  File file = LittleFS.open("/schedules.json", "w");
  if (!file) {
    Serial.println("Error abriendo archivo para escribir");
    return;
  }

  serializeJson(doc, file);
  file.close();
  Serial.println("Horarios guardados");
}

void loadNormalUser() {
  File file = LittleFS.open("/user.json", "r");
  if (!file) {
    Serial.println("No hay usuario normal configurado");
    normalUserConfigured = false;
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Error leyendo usuario");
    normalUserConfigured = false;
    return;
  }

  normalUser = doc["username"].as<String>();
  normalPass = doc["password"].as<String>();
  normalUserConfigured = (normalUser.length() > 0);

  if (normalUserConfigured) {
    Serial.println("Usuario normal cargado");
  }
}

void saveNormalUser(const String& username, const String& password) {
  JsonDocument doc;
  doc["username"] = username;
  doc["password"] = password;

  File file = LittleFS.open("/user.json", "w");
  if (!file) {
    Serial.println("Error guardando usuario");
    return;
  }

  serializeJson(doc, file);
  file.close();

  normalUser = username;
  normalPass = password;
  normalUserConfigured = true;

  Serial.println("Usuario normal guardado");
}

void activateBell(int bellIndex) {
  if (bellIndex < 0 || bellIndex >= 4) return;

  // Verificar si el sistema global está activado
  if (!globalSystemEnabled) {
    Serial.printf("Sistema desactivado. %s no se activará.\n", bellNames[bellIndex].c_str());
    return;
  }

  digitalWrite(RELAY_PINS[bellIndex], HIGH);
  bellActive[bellIndex] = true;
  bellStartTime[bellIndex] = millis();
  Serial.printf("%s activado\n", bellNames[bellIndex].c_str());

  // Publicar evento MQTT
  publishBellEvent(bellIndex, "activated");
}

void updateBells() {
  unsigned long currentTime = millis();

  for (int i = 0; i < 4; i++) {
    if (bellActive[i] && (currentTime - bellStartTime[i] >= bellDurations[i])) {
      digitalWrite(RELAY_PINS[i], LOW);
      bellActive[i] = false;
      Serial.printf("%s desactivado\n", bellNames[i].c_str());

      // Publicar evento MQTT
      publishBellEvent(i, "deactivated");
    }
  }
}

void checkSchedules() {
  if (!hasValidTime()) return; // No verificar horarios si no hay fuente de tiempo

  DateTime now = getCurrentTime();
  int currentMinute = now.hour() * 60 + now.minute();

  if (currentMinute == lastCheckMinute) return;
  lastCheckMinute = currentMinute;

  int dayOfWeek = now.dayOfTheWeek();
  int adjustedDay = (dayOfWeek + 6) % 7;

  for (int i = 0; i < scheduleCount; i++) {
    if (!schedules[i].enabled) continue;

    if (schedules[i].hour == now.hour() &&
        schedules[i].minute == now.minute() &&
        schedules[i].days[adjustedDay]) {

      activateBell(schedules[i].bellIndex);
    }
  }
}

// ============== MANEJADORES WEB SERVER ==============

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Archivo no encontrado");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleLoginPage() {
  File file = LittleFS.open("/login.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Archivo no encontrado");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleConfig() {
  // Servir config.html sin verificar auth en backend
  // La verificación se hace en el frontend via /api/check-auth
  File file = LittleFS.open("/config.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Archivo no encontrado");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleWiFiConfig() {
  File file = LittleFS.open("/wifi_config.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Archivo no encontrado");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleGetBellNames() {
  JsonDocument doc;
  JsonArray array = doc["names"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellNames[i]);
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveBellNames() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  JsonArray array = doc["names"].as<JsonArray>();
  for (int i = 0; i < 4 && i < array.size(); i++) {
    bellNames[i] = array[i].as<String>();
  }

  saveBellNames();
  server.send(200, "application/json", "{\"success\":true}");
}

void handleGetBellDurations() {
  JsonDocument doc;
  JsonArray array = doc["durations"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellDurations[i]);
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveBellDurations() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  JsonArray array = doc["durations"].as<JsonArray>();
  for (int i = 0; i < 4 && i < array.size(); i++) {
    int duration = array[i].as<int>();
    // Validar que la duración esté entre 100ms y 60000ms (1 minuto)
    if (duration >= 100 && duration <= 60000) {
      bellDurations[i] = duration;
    }
  }

  saveBellDurations();
  server.send(200, "application/json", "{\"success\":true}");
}

void handleScanNetworks() {
  Serial.println("[WiFi Scan] Iniciando escaneo de redes...");

  // Escanear redes (esto puede tardar varios segundos)
  int n = WiFi.scanNetworks(false, true); // async=false, show_hidden=true

  Serial.printf("[WiFi Scan] Escaneo completado. Redes encontradas: %d\n", n);

  JsonDocument doc;
  JsonArray networks = doc.to<JsonArray>();

  if (n == 0) {
    Serial.println("[WiFi Scan] No se encontraron redes");
  } else if (n > 0) {
    for (int i = 0; i < n; i++) {
      JsonObject network = networks.add<JsonObject>();
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);

      network["ssid"] = ssid;
      network["rssi"] = rssi;
      network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "encrypted";

      Serial.printf("[WiFi Scan] %d: %s (%d dBm)\n", i + 1, ssid.c_str(), rssi);
    }
  } else {
    Serial.printf("[WiFi Scan] Error en el escaneo: código %d\n", n);
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);

  // Limpiar resultados del escaneo para liberar memoria
  WiFi.scanDelete();
}

void handleSaveWiFi() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  String ssid = doc["ssid"].as<String>();
  String password = doc["password"].as<String>();

  if (ssid.length() == 0) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"SSID vacío\"}");
    return;
  }

  saveWiFiConfig(ssid, password);

  server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuración guardada. Intentando conectar...\"}");

  // Intentar conectar después de un pequeño delay
  delay(500);
  if (connectToWiFi()) {
    Serial.println("Conectado exitosamente al nuevo WiFi");
  } else {
    Serial.println("No se pudo conectar, AP permanece activo");
  }
}

void handleGetWiFiStatus() {
  JsonDocument doc;

  doc["mode"] = (currentMode == MODE_AP) ? "AP" : "STA";
  doc["connected"] = (WiFi.status() == WL_CONNECTED);
  doc["ap_ssid"] = ap_ssid;
  doc["ap_ip"] = WiFi.softAPIP().toString();

  if (WiFi.status() == WL_CONNECTED) {
    doc["sta_ssid"] = WiFi.SSID();
    doc["sta_ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
  }

  if (wifiConfigured) {
    doc["saved_ssid"] = saved_ssid;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetSchedules() {
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < scheduleCount; i++) {
    JsonObject obj = array.add<JsonObject>();
    obj["id"] = i;
    obj["name"] = schedules[i].name;
    obj["enabled"] = schedules[i].enabled;
    obj["hour"] = schedules[i].hour;
    obj["minute"] = schedules[i].minute;
    obj["bell"] = schedules[i].bellIndex;

    JsonArray daysArray = obj["days"].to<JsonArray>();
    for (int j = 0; j < 7; j++) {
      daysArray.add(schedules[i].days[j]);
    }
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveSchedules() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  JsonArray array = doc.as<JsonArray>();
  scheduleCount = 0;

  for (JsonObject obj : array) {
    if (scheduleCount >= MAX_SCHEDULES) break;

    // Guardar nombre del horario
    String scheduleName = obj["name"] | "";
    strncpy(schedules[scheduleCount].name, scheduleName.c_str(), sizeof(schedules[scheduleCount].name) - 1);
    schedules[scheduleCount].name[sizeof(schedules[scheduleCount].name) - 1] = '\0';

    schedules[scheduleCount].enabled = obj["enabled"] | true;
    schedules[scheduleCount].hour = obj["hour"];
    schedules[scheduleCount].minute = obj["minute"];
    schedules[scheduleCount].bellIndex = obj["bell"];

    JsonArray daysArray = obj["days"];
    for (int i = 0; i < 7; i++) {
      schedules[scheduleCount].days[i] = daysArray[i];
    }

    scheduleCount++;
  }

  saveSchedules();
  server.send(200, "application/json", "{\"success\":true}");
}

void handleGetTime() {
  if (!hasValidTime()) {
    server.send(503, "application/json", "{\"error\":\"No hay fuente de tiempo disponible\"}");
    return;
  }

  DateTime now = getCurrentTime();

  JsonDocument doc;
  doc["year"] = now.year();
  doc["month"] = now.month();
  doc["day"] = now.day();
  doc["hour"] = now.hour();
  doc["minute"] = now.minute();
  doc["second"] = now.second();
  doc["dayOfWeek"] = now.dayOfTheWeek();
  doc["source"] = rtcAvailable ? "RTC" : "NTP";

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSetTime() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  DateTime newTime(
    doc["year"],
    doc["month"],
    doc["day"],
    doc["hour"],
    doc["minute"],
    doc["second"]
  );

  // Actualizar RTC si está disponible
  if (rtcAvailable) {
    rtc.adjust(newTime);
  }

  // Actualizar tiempo NTP (siempre, como respaldo)
  ntpEpochTime = newTime.unixtime();
  ntpLastUpdate = millis();
  ntpTimeValid = true;

  server.send(200, "application/json", "{\"success\":true}");
}

void handleTestBell() {
  if (!server.hasArg("bell")) {
    server.send(400, "text/plain", "Falta parámetro 'bell'");
    return;
  }

  int bellIndex = server.arg("bell").toInt();
  if (bellIndex < 0 || bellIndex >= 4) {
    server.send(400, "text/plain", "Índice de timbre inválido");
    return;
  }

  activateBell(bellIndex);
  server.send(200, "application/json", "{\"success\":true}");
}

void handleSyncNTP() {
  if (WiFi.status() != WL_CONNECTED) {
    server.send(503, "application/json", "{\"success\":false,\"error\":\"WiFi no conectado\"}");
    return;
  }

  if (timeClient.update()) {
    unsigned long epochTime = timeClient.getEpochTime();

    // Actualizar RTC si está disponible
    if (rtcAvailable) {
      rtc.adjust(DateTime(epochTime));
    }

    // Actualizar tiempo NTP (siempre, como respaldo)
    ntpEpochTime = epochTime;
    ntpLastUpdate = millis();
    ntpTimeValid = true;

    Serial.println("Hora sincronizada desde NTP");
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(503, "application/json", "{\"success\":false,\"error\":\"Error sincronizando con NTP\"}");
  }
}

// Autenticación - Login
void handleLogin() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  String username = doc["username"].as<String>();
  String password = doc["password"].as<String>();

  bool isAdmin = false;
  bool isValid = false;

  // Verificar admin
  if (username == ADMIN_USER && password == ADMIN_PASS) {
    isValid = true;
    isAdmin = true;
  }
  // Verificar usuario normal
  else if (normalUserConfigured && username == normalUser && password == normalPass) {
    isValid = true;
    isAdmin = false;
  }

  if (isValid) {
    activeSession = generateToken();
    sessionExpiry = millis() + SESSION_DURATION;
    activeSessionIsAdmin = isAdmin;

    JsonDocument response;
    response["success"] = true;
    response["token"] = activeSession;
    response["isAdmin"] = isAdmin;

    String responseStr;
    serializeJson(response, responseStr);
    server.send(200, "application/json", responseStr);

    Serial.printf("Login exitoso: %s (admin: %s)\n", username.c_str(), isAdmin ? "si" : "no");
  } else {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"Credenciales inválidas\"}");
    Serial.println("Intento de login fallido");
  }
}

// Verificar sesión
void handleCheckAuth() {
  if (checkAuth()) {
    JsonDocument response;
    response["authenticated"] = true;
    response["isAdmin"] = activeSessionIsAdmin;

    String responseStr;
    serializeJson(response, responseStr);
    server.send(200, "application/json", responseStr);
  } else {
    server.send(401, "application/json", "{\"authenticated\":false}");
  }
}

// Logout
void handleLogout() {
  activeSession = "";
  sessionExpiry = 0;
  activeSessionIsAdmin = false;
  server.send(200, "application/json", "{\"success\":true}");
}

// Crear/Actualizar usuario normal (solo admin)
void handleSaveNormalUser() {
  if (!checkAdminAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"Solo admin puede modificar usuarios\"}");
    return;
  }

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  String username = doc["username"].as<String>();
  String password = doc["password"].as<String>();

  // Si username está vacío, eliminar usuario
  if (username.length() == 0) {
    // Eliminar archivo de usuario
    LittleFS.remove("/user.json");
    normalUser = "";
    normalPass = "";
    normalUserConfigured = false;
    Serial.println("Usuario normal eliminado");
    server.send(200, "application/json", "{\"success\":true}");
    return;
  }

  // Si hay contraseña, actualizar ambos
  if (password.length() > 0) {
    saveNormalUser(username, password);
  }
  // Si no hay contraseña pero ya existe un usuario, solo actualizar username
  else if (normalUserConfigured) {
    saveNormalUser(username, normalPass);
  }
  // Si no hay contraseña y no existe usuario, error
  else {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Contraseña requerida para nuevo usuario\"}");
    return;
  }

  server.send(200, "application/json", "{\"success\":true}");
}

// Obtener info del usuario normal
void handleGetNormalUser() {
  if (!checkAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"No autenticado\"}");
    return;
  }

  JsonDocument doc;
  doc["configured"] = normalUserConfigured;
  if (normalUserConfigured) {
    doc["username"] = normalUser;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Obtener visibilidad de timbres
void handleGetBellVisibility() {
  JsonDocument doc;
  JsonArray array = doc["visibility"].to<JsonArray>();

  for (int i = 0; i < 4; i++) {
    array.add(bellVisibility[i]);
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Guardar visibilidad de timbres
void handleSaveBellVisibility() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  JsonArray array = doc["visibility"].as<JsonArray>();

  // Validar que al menos un timbre esté visible
  bool atLeastOneVisible = false;
  for (int i = 0; i < 4 && i < array.size(); i++) {
    bool visible = array[i].as<bool>();
    bellVisibility[i] = visible;
    if (visible) atLeastOneVisible = true;
  }

  if (!atLeastOneVisible) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Al menos un timbre debe estar visible\"}");
    return;
  }

  saveBellVisibility();
  server.send(200, "application/json", "{\"success\":true}");
}

// Crear backup completo del sistema
void handleBackup() {
  if (!checkAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"No autenticado\"}");
    return;
  }

  JsonDocument doc;

  // Información de versión y timestamp
  doc["version"] = "1.0";
  doc["timestamp"] = millis();
  doc["device"] = "Tora-Or";

  // Nombres de timbres
  JsonArray namesArray = doc["bellNames"].to<JsonArray>();
  for (int i = 0; i < 4; i++) {
    namesArray.add(bellNames[i]);
  }

  // Duraciones de timbres
  JsonArray durationsArray = doc["bellDurations"].to<JsonArray>();
  for (int i = 0; i < 4; i++) {
    durationsArray.add(bellDurations[i]);
  }

  // Horarios programados
  JsonArray schedulesArray = doc["schedules"].to<JsonArray>();
  for (int i = 0; i < scheduleCount; i++) {
    JsonObject obj = schedulesArray.add<JsonObject>();
    obj["name"] = schedules[i].name;
    obj["enabled"] = schedules[i].enabled;
    obj["hour"] = schedules[i].hour;
    obj["minute"] = schedules[i].minute;
    obj["bell"] = schedules[i].bellIndex;

    JsonArray daysArray = obj["days"].to<JsonArray>();
    for (int j = 0; j < 7; j++) {
      daysArray.add(schedules[i].days[j]);
    }
  }

  // Configuración WiFi (incluye SSID y contraseña)
  if (wifiConfigured) {
    doc["wifiConfigured"] = true;
    doc["wifiSSID"] = saved_ssid;
    doc["wifiPassword"] = saved_password;
  } else {
    doc["wifiConfigured"] = false;
  }

  // Visibilidad de timbres
  JsonArray visibilityArray = doc["bellVisibility"].to<JsonArray>();
  for (int i = 0; i < 4; i++) {
    visibilityArray.add(bellVisibility[i]);
  }

  // Usuario normal (incluye username y contraseña)
  if (normalUserConfigured) {
    doc["normalUserConfigured"] = true;
    doc["normalUsername"] = normalUser;
    doc["normalPassword"] = normalPass;
  } else {
    doc["normalUserConfigured"] = false;
  }

  // Configuración MQTT (incluye server, puerto, usuario y contraseña)
  if (mqtt_enabled) {
    doc["mqttConfigured"] = true;
    doc["mqttServer"] = mqtt_server;
    doc["mqttPort"] = mqtt_port;
    doc["mqttUser"] = mqtt_user;
    doc["mqttPassword"] = mqtt_password;
    doc["mqttClientId"] = mqtt_client_id;
    doc["mqttEnabled"] = mqtt_enabled;
    doc["mqttHADiscovery"] = mqtt_ha_discovery;
  } else {
    doc["mqttConfigured"] = false;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
  Serial.println("Backup generado");
}

// Restaurar backup del sistema
void handleRestore() {
  if (!checkAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"No autenticado\"}");
    return;
  }

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"JSON inválido\"}");
    return;
  }

  // Validar que es un backup válido
  if (!doc.containsKey("version") || !doc.containsKey("device")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Formato de backup inválido\"}");
    return;
  }

  bool success = true;

  // Restaurar nombres de timbres
  if (doc.containsKey("bellNames")) {
    JsonArray namesArray = doc["bellNames"].as<JsonArray>();
    for (int i = 0; i < 4 && i < namesArray.size(); i++) {
      bellNames[i] = namesArray[i].as<String>();
    }
    saveBellNames();
  }

  // Restaurar duraciones de timbres
  if (doc.containsKey("bellDurations")) {
    JsonArray durationsArray = doc["bellDurations"].as<JsonArray>();
    for (int i = 0; i < 4 && i < durationsArray.size(); i++) {
      bellDurations[i] = durationsArray[i].as<int>();
    }
    saveBellDurations();
  }

  // Restaurar horarios
  if (doc.containsKey("schedules")) {
    JsonArray schedulesArray = doc["schedules"].as<JsonArray>();
    scheduleCount = 0;

    for (JsonObject obj : schedulesArray) {
      if (scheduleCount >= MAX_SCHEDULES) break;

      // Restaurar nombre del horario
      String scheduleName = obj["name"] | "";
      strncpy(schedules[scheduleCount].name, scheduleName.c_str(), sizeof(schedules[scheduleCount].name) - 1);
      schedules[scheduleCount].name[sizeof(schedules[scheduleCount].name) - 1] = '\0';

      schedules[scheduleCount].enabled = obj["enabled"] | true;
      schedules[scheduleCount].hour = obj["hour"];
      schedules[scheduleCount].minute = obj["minute"];
      schedules[scheduleCount].bellIndex = obj["bell"];

      JsonArray daysArray = obj["days"];
      for (int i = 0; i < 7; i++) {
        schedules[scheduleCount].days[i] = daysArray[i];
      }

      scheduleCount++;
    }
    saveSchedules();
  }

  // Restaurar configuración WiFi (SSID y contraseña)
  if (doc["wifiConfigured"].as<bool>() && doc.containsKey("wifiSSID")) {
    String ssid = doc["wifiSSID"].as<String>();
    String password = doc.containsKey("wifiPassword") ? doc["wifiPassword"].as<String>() : "";

    if (ssid.length() > 0) {
      saveWiFiConfig(ssid, password);
      Serial.printf("WiFi del backup restaurado: %s\n", ssid.c_str());

      // Intentar conectar
      if (connectToWiFi()) {
        Serial.println("Conectado exitosamente a WiFi restaurado");
      }
    }
  }

  // Restaurar visibilidad de timbres
  if (doc.containsKey("bellVisibility")) {
    JsonArray visibilityArray = doc["bellVisibility"].as<JsonArray>();
    for (int i = 0; i < 4 && i < visibilityArray.size(); i++) {
      bellVisibility[i] = visibilityArray[i].as<bool>();
    }
    saveBellVisibility();
    Serial.println("Visibilidad de timbres restaurada");
  }

  // Restaurar usuario normal (username y contraseña)
  if (doc["normalUserConfigured"].as<bool>() && doc.containsKey("normalUsername")) {
    String username = doc["normalUsername"].as<String>();
    String password = doc["normalPassword"].as<String>();

    if (username.length() > 0 && password.length() > 0) {
      normalUser = username;
      normalPass = password;
      normalUserConfigured = true;
      saveNormalUser(username, password);
      Serial.printf("Usuario del backup restaurado: %s\n", username.c_str());
    }
  }

  // Restaurar configuración MQTT
  if (doc["mqttConfigured"].as<bool>()) {
    mqtt_server = doc["mqttServer"].as<String>();
    mqtt_port = doc["mqttPort"] | 1883;
    mqtt_user = doc["mqttUser"].as<String>();
    mqtt_password = doc["mqttPassword"].as<String>();
    mqtt_client_id = doc["mqttClientId"] | "timbres-tora-or";
    mqtt_enabled = doc["mqttEnabled"] | false;
    mqtt_ha_discovery = doc["mqttHADiscovery"] | true;

    saveMQTTConfig();
    Serial.println("Configuración MQTT restaurada");

    // Reconectar si está habilitado y hay WiFi
    if (mqtt_enabled && mqtt_server.length() > 0 && WiFi.status() == WL_CONNECTED) {
      mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
      reconnectMQTT();
    }
  }

  if (success) {
    server.send(200, "application/json", "{\"success\":true}");
    Serial.println("Backup restaurado correctamente");
  } else {
    server.send(500, "application/json", "{\"success\":false,\"error\":\"Error restaurando algunos datos\"}");
  }
}

// Página HTML para OTA update
void handleOTAPage() {
  // OTA page is now accessible without authentication
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Actualización OTA - Tora Or</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 40px;
            max-width: 600px;
            width: 100%;
        }
        h1 { color: #667eea; margin-bottom: 10px; }
        .warning {
            background: #fff3cd;
            border: 2px solid #ffc107;
            border-radius: 10px;
            padding: 15px;
            margin: 20px 0;
            color: #856404;
        }
        .warning-title { font-weight: 600; margin-bottom: 5px; }
        .form-group { margin: 20px 0; }
        .form-group label {
            display: block;
            margin-bottom: 10px;
            color: #4a5568;
            font-weight: 600;
        }
        input[type="file"] {
            width: 100%;
            padding: 10px;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
        }
        .btn {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            padding: 15px 30px;
            border-radius: 8px;
            font-size: 1.1em;
            cursor: pointer;
            width: 100%;
            margin-top: 10px;
            font-weight: 600;
        }
        .btn:hover { opacity: 0.9; }
        .btn:disabled { opacity: 0.5; cursor: not-allowed; }
        .progress-container {
            display: none;
            margin-top: 20px;
        }
        .progress-bar {
            width: 100%;
            height: 30px;
            background: #e2e8f0;
            border-radius: 15px;
            overflow: hidden;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            width: 0%;
            transition: width 0.3s;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: 600;
        }
        .status { margin-top: 15px; text-align: center; color: #4a5568; font-weight: 600; }
        .success { color: #48bb78; }
        .error { color: #f56565; }
        .back-btn {
            display: inline-block;
            margin-top: 20px;
            color: #667eea;
            text-decoration: none;
            font-weight: 600;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔄 Actualización OTA</h1>
        <p style="color: #718096; margin-bottom: 20px;">Actualiza el firmware del sistema sin perder la configuración</p>

        <div class="warning">
            <div class="warning-title">⚠️ Importante</div>
            <ul style="margin-left: 20px; margin-top: 10px;">
                <li>Solo archivos .bin generados para este dispositivo</li>
                <li>No apagues el dispositivo durante la actualización</li>
                <li>La configuración se mantendrá después de la actualización</li>
                <li>El proceso puede tardar 1-2 minutos</li>
            </ul>
        </div>

        <form id="otaForm">
            <div class="form-group">
                <label>Seleccionar archivo de firmware (.bin)</label>
                <input type="file" id="firmwareFile" accept=".bin" required>
            </div>
            <button type="submit" class="btn" id="uploadBtn">Actualizar Firmware</button>
        </form>

        <div class="progress-container" id="progressContainer">
            <div class="progress-bar">
                <div class="progress-fill" id="progressFill">0%</div>
            </div>
            <div class="status" id="statusText">Subiendo firmware...</div>
        </div>

        <a href="/config.html" class="back-btn">← Volver a Configuración</a>
    </div>

    <script>
        function getAuthToken() {
            return localStorage.getItem('authToken');
        }

        document.getElementById('otaForm').addEventListener('submit', async function(e) {
            e.preventDefault();

            const fileInput = document.getElementById('firmwareFile');
            const file = fileInput.files[0];

            if (!file) {
                alert('Por favor selecciona un archivo');
                return;
            }

            if (!file.name.endsWith('.bin')) {
                alert('Solo se permiten archivos .bin');
                return;
            }

            if (!confirm('¿Estás seguro de actualizar el firmware? El dispositivo se reiniciará.')) {
                return;
            }

            const uploadBtn = document.getElementById('uploadBtn');
            const progressContainer = document.getElementById('progressContainer');
            const progressFill = document.getElementById('progressFill');
            const statusText = document.getElementById('statusText');

            uploadBtn.disabled = true;
            progressContainer.style.display = 'block';

            try {
                const formData = new FormData();
                formData.append('firmware', file);

                const xhr = new XMLHttpRequest();

                xhr.upload.addEventListener('progress', (e) => {
                    if (e.lengthComputable) {
                        const percent = Math.round((e.loaded / e.total) * 100);
                        progressFill.style.width = percent + '%';
                        progressFill.textContent = percent + '%';
                    }
                });

                xhr.addEventListener('load', () => {
                    if (xhr.status === 200) {
                        progressFill.style.width = '100%';
                        progressFill.textContent = '100%';
                        statusText.textContent = '✓ Actualización completada. Reiniciando dispositivo...';
                        statusText.className = 'status success';

                        setTimeout(() => {
                            window.location.href = '/';
                        }, 5000);
                    } else {
                        statusText.textContent = '✗ Error en la actualización';
                        statusText.className = 'status error';
                        uploadBtn.disabled = false;
                    }
                });

                xhr.addEventListener('error', () => {
                    statusText.textContent = '✗ Error de conexión';
                    statusText.className = 'status error';
                    uploadBtn.disabled = false;
                });

                xhr.open('POST', '/api/ota/update');
                xhr.setRequestHeader('Authorization', getAuthToken());
                xhr.send(formData);

            } catch (error) {
                statusText.textContent = '✗ Error: ' + error.message;
                statusText.className = 'status error';
                uploadBtn.disabled = false;
            }
        });
    </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Manejar actualización OTA
void handleOTAUpdate() {
  // OTA update is now accessible without authentication
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Actualización OTA: %s\n", upload.filename.c_str());

    // Comenzar actualización OTA
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    // Escribir datos del firmware
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("Actualización exitosa: %u bytes\n", upload.totalSize);
      server.send(200, "text/plain", "OK");
      delay(1000);
      ESP.restart();
    } else {
      Update.printError(Serial);
      server.send(500, "text/plain", "Error en actualización");
    }
  }
}

// Servir archivos estáticos (logo PNG)
void handleLogo() {
  File file = LittleFS.open("/logo-tora-or.png", "r");
  if (!file) {
    Serial.println("[ERROR] No se pudo abrir logo-tora-or.png");
    server.send(404, "text/plain", "Logo no encontrado");
    return;
  }

  server.streamFile(file, "image/png");
  file.close();
}

void handlePoweredLight() {
  File file = LittleFS.open("/poweredlight.png", "r");
  if (!file) {
    Serial.println("[ERROR] No se pudo abrir poweredlight.png");
    server.send(404, "text/plain", "Logo no encontrado");
    return;
  }

  server.streamFile(file, "image/png");
  file.close();
}

// Captive portal - redirigir todo al portal de configuración
void handleNotFound() {
  Serial.printf("[DEBUG] Ruta no encontrada: %s (Método: %s)\n",
                server.uri().c_str(),
                (server.method() == HTTP_GET) ? "GET" :
                (server.method() == HTTP_POST) ? "POST" : "OTHER");

  if (currentMode == MODE_AP) {
    // Redirigir al portal de configuración WiFi
    server.sendHeader("Location", "http://192.168.4.1/wifi_config.html", true);
    server.send(302, "text/plain", "");
  } else {
    server.send(404, "text/plain", "Not Found");
  }
}

// Handler para obtener configuración MQTT
void handleGetMQTT() {
  if (!checkAdminAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"Solo admin puede ver configuración MQTT\"}");
    return;
  }

  JsonDocument doc;
  doc["server"] = mqtt_server;
  doc["port"] = mqtt_port;
  doc["user"] = mqtt_user;
  doc["client_id"] = mqtt_client_id;
  doc["enabled"] = mqtt_enabled;
  doc["ha_discovery"] = mqtt_ha_discovery;
  doc["connected"] = mqttClient.connected();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Handler para guardar configuración MQTT
void handleSaveMQTT() {
  if (!checkAdminAuth()) {
    server.send(401, "application/json", "{\"success\":false,\"error\":\"Solo admin puede modificar MQTT\"}");
    return;
  }

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No se recibieron datos");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  mqtt_server = doc["server"].as<String>();
  mqtt_port = doc["port"] | 1883;
  mqtt_user = doc["user"].as<String>();
  mqtt_password = doc["password"].as<String>();
  mqtt_client_id = doc["client_id"] | "timbres-tora-or";
  mqtt_enabled = doc["enabled"] | false;
  mqtt_ha_discovery = doc["ha_discovery"] | true;

  saveMQTTConfig();

  if (mqtt_enabled && mqtt_server.length() > 0) {
    mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
    if (WiFi.status() == WL_CONNECTED) {
      reconnectMQTT();
    }
  } else if (!mqtt_enabled && mqttClient.connected()) {
    mqttClient.disconnect();
  }

  server.send(200, "application/json", "{\"success\":true}");
}

void initWebServer() {
  // Rutas de autenticación
  server.on("/login.html", handleLoginPage);
  server.on("/api/auth/login", HTTP_POST, handleLogin);
  server.on("/api/auth/check", HTTP_GET, handleCheckAuth);
  server.on("/api/auth/logout", HTTP_POST, handleLogout);
  server.on("/api/user/normal", HTTP_GET, handleGetNormalUser);
  server.on("/api/user/normal", HTTP_POST, handleSaveNormalUser);

  // Rutas para configuración WiFi
  server.on("/wifi_config.html", handleWiFiConfig);
  server.on("/api/wifi/scan", HTTP_GET, handleScanNetworks);
  server.on("/api/wifi/save", HTTP_POST, handleSaveWiFi);
  server.on("/api/wifi/status", HTTP_GET, handleGetWiFiStatus);

  // Rutas para configuración MQTT
  server.on("/api/mqtt/config", HTTP_GET, handleGetMQTT);
  server.on("/api/mqtt/config", HTTP_POST, handleSaveMQTT);

  // Servir archivos estáticos
  server.on("/logo-tora-or.png", HTTP_GET, handleLogo);
  server.on("/poweredlight.png", HTTP_GET, handlePoweredLight);

  // Rutas para el panel principal
  server.on("/", handleRoot);
  server.on("/config.html", handleConfig);
  server.on("/api/bell-names", HTTP_GET, handleGetBellNames);
  server.on("/api/bell-names", HTTP_POST, handleSaveBellNames);
  server.on("/api/bell-durations", HTTP_GET, handleGetBellDurations);
  server.on("/api/bell-durations", HTTP_POST, handleSaveBellDurations);
  server.on("/api/bell-visibility", HTTP_GET, handleGetBellVisibility);
  server.on("/api/bell-visibility", HTTP_POST, handleSaveBellVisibility);
  server.on("/api/schedules", HTTP_GET, handleGetSchedules);
  server.on("/api/schedules", HTTP_POST, handleSaveSchedules);
  server.on("/api/time", HTTP_GET, handleGetTime);
  server.on("/api/time", HTTP_POST, handleSetTime);
  server.on("/api/test", HTTP_POST, handleTestBell);
  server.on("/api/sync", HTTP_POST, handleSyncNTP);

  // Rutas para backup/restore
  server.on("/api/backup", HTTP_GET, handleBackup);
  server.on("/api/restore", HTTP_POST, handleRestore);

  // Rutas para OTA
  server.on("/ota.html", HTTP_GET, handleOTAPage);
  server.on("/api/ota/update", HTTP_POST, []() {
    server.send(200, "text/plain", "OK");
  }, handleOTAUpdate);

  // Captive portal
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor web iniciado");
}

// ============== SETUP Y LOOP ==============

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Sistema de Timbres Tora Or ===");
  Serial.println("=== Con Captive Portal WiFi ===\n");

  initRelays();
  initRTC();
  initFileSystem();
  initWiFi();
  initWebServer();

  // Configurar callback MQTT
  mqttClient.setCallback(mqttCallback);

  // Intentar conectar a MQTT si está habilitado y hay WiFi
  if (mqtt_enabled && WiFi.status() == WL_CONNECTED) {
    reconnectMQTT();
  }

  Serial.println("\n=== Sistema listo ===");

  if (currentMode == MODE_AP) {
    Serial.println("Modo: Access Point");
    Serial.printf("Conéctate a: %s\n", ap_ssid);
    Serial.printf("Contraseña: %s\n", ap_password);
    Serial.println("Abre: http://192.168.4.1/config");
  } else {
    Serial.println("Modo: WiFi Cliente");
    Serial.printf("WiFi: %s\n", WiFi.SSID().c_str());
    Serial.printf("Panel: http://%s\n", WiFi.localIP().toString().c_str());
  }

  if (mqtt_enabled) {
    Serial.printf("MQTT: %s (estado: %s)\n", mqtt_enabled ? "Habilitado" : "Deshabilitado",
                  mqttClient.connected() ? "Conectado" : "Desconectado");
  }
}

void loop() {
  // Procesar DNS server para captive portal
  if (currentMode == MODE_AP) {
    dnsServer.processNextRequest();
  }

  server.handleClient();
  updateBells();
  checkSchedules();
  checkWiFiConnection();

  // Manejar conexión MQTT
  handleMQTTConnection();

  // Publicar sensores MQTT cada 30 segundos
  static unsigned long lastSensorPublish = 0;
  if (mqttClient.connected() && millis() - lastSensorPublish > 30000) {
    publishMQTTSensors();
    lastSensorPublish = millis();
  }

  // Sincronizar con NTP cada hora si hay WiFi (con o sin RTC)
  static unsigned long lastNTPSync = 0;
  if (WiFi.status() == WL_CONNECTED && millis() - lastNTPSync > 3600000) {
    if (timeClient.update()) {
      unsigned long epochTime = timeClient.getEpochTime();

      // Actualizar RTC si está disponible
      if (rtcAvailable) {
        rtc.adjust(DateTime(epochTime));
        Serial.println("RTC sincronizado con NTP");
      }

      // Actualizar tiempo NTP en memoria (siempre)
      ntpEpochTime = epochTime;
      ntpLastUpdate = millis();
      ntpTimeValid = true;
      if (!rtcAvailable) {
        Serial.println("Tiempo NTP actualizado (sin RTC)");
      }
    }
    lastNTPSync = millis();
  }

  delay(50);
}
