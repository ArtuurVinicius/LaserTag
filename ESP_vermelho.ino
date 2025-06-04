#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

// LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2); // I2C endereço 0x3F

// IR
const uint16_t RECV_PIN = 14; // D5
IRrecv irrecv(RECV_PIN);
decode_results results;

// LEDs
#define LED_GREEN 0    // D3
#define LED_YELLOW 2   // D4
#define LED_RED 12     // D6

int health = 10;
int partidas = 0;
int vitorias = 0;

// IR Codes
const uint32_t BOTAO_SETA_CIMA = 0xFF02FD;
const uint32_t BOTAO_POWER = 0xFFA25D;

// Wi-Fi e MQTT
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define MQTT_HOST IPAddress(192, 168, 66, 163)  // IP da VM
#define MQTT_PORT 1883
#define MQTT_USER "guri"
#define MQTT_PASS "normal123"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

// ========================
// Setup
// ========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // LCD
  Wire.begin(5, 4); // SDA = GPIO5 (D1), SCL = GPIO4 (D2)
  lcd.init();
  lcd.backlight();

  // IR
  irrecv.enableIRIn();

  // LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Wi-Fi e MQTT
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASS);

  connectToWifi();

  updateLCD();
  updateLEDs();
}

// ========================
// Loop principal
// ========================
void loop() {
  if (irrecv.decode(&results)) {
    // Serial.print("IR: 0x");
    // Serial.println(results.value, HEX);

    if (results.value == BOTAO_SETA_CIMA && health > 0) {
      health--;
      if (health == 0) {
        partidas++;
        publishStats();
      }
      updateLCD();
      updateLEDs();
    }

    if (results.value == BOTAO_POWER && health == 0) {
      health = 10;
      vitorias++;
      updateLCD();
      updateLEDs();
      publishStats();
    }

    irrecv.resume();
    delay(250);
  }
}

// ========================
// Conexões Wi-Fi e MQTT
// ========================
void connectToWifi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Wi-Fi conectado!");
  mqttClient.connect();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Wi-Fi desconectado.");
  mqttReconnectTimer.detach();
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Conectado ao broker MQTT!");
  publishStats(); // publica ao conectar
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("MQTT desconectado.");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, []() { mqttClient.connect(); });
  }
}

// ========================
// Publicação MQTT
// ========================
void publishStats() {
  mqttClient.publish("vitorias/azul", 1, true, String(vitorias).c_str());
  //mqttClient.publish("partidas", 1, true, String(partidas).c_str());
  Serial.println("Estatísticas publicadas.");
}

// ========================
// LCD e LEDs
// ========================
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (health == 0) {
    lcd.print("Derrota...");
    lcd.setCursor(0, 1);
    lcd.print("Fim de jogo!");
  } else {
    lcd.print("Vida: ");
    lcd.print(health);
  }
}

void updateLEDs() {
  if (health > 6) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } else if (health > 3) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  } else if (health > 0) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }
}