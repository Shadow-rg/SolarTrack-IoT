#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>

// ==================================================
// CONFIGURACIÓN WIFI
// ==================================================
const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";

// ==================================================
// CONFIGURACIÓN MQTT
// ==================================================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ==================================================
// BUFFER LOCAL PARA DATOS PENDIENTES
// ==================================================
std::vector<String> buffer; // almacena mensajes cuando no hay internet

// ==================================================
// FUNCIÓN: Conexión a WiFi
// ==================================================
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// ==================================================
// FUNCIÓN: Reconexión a MQTT
// ==================================================
void reconnect() {
  while(!client.connected()) {
    if(client.connect("SolarTrackESP32")) {
      // Conectado correctamente
    } else {
      delay(2000); // esperar antes de reintentar
    }
  }
}

// ==================================================
// SETUP
// ==================================================
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

// ==================================================
// LOOP PRINCIPAL
// ==================================================
void loop() {
  // Verificar conexión MQTT
  if(!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer datos del Serial (desde Arduino SolarTrack)
  if(Serial.available()) {
    String payload = Serial.readStringUntil('\n');

    if(client.connected()) {
      // Si hay internet → publicar directamente
      client.publish("solartrack/data", payload.c_str());
    } else {
      // Si no hay internet → guardar en buffer
      buffer.push_back(payload);
    }
  }

  // Si hay conexión y buffer no está vacío → enviar pendientes
  if(client.connected() && !buffer.empty()) {
    for(String msg : buffer) {
      client.publish("solartrack/data", msg.c_str());
    }
    buffer.clear(); // limpiar buffer después de enviar
  }
}