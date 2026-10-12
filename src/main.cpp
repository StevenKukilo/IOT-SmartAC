#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ====== PENGATURAN ======
const char* ID        = "acpintar-StevenKukilo";   // GANTI dengan kode unik kamu (sama dengan di dashboard)
const char* WIFI_SSID = "Wokwi-GUEST";
const char* MQTT_HOST = "broker.hivemq.com";

#define DHT_PIN   4
#define PIR_PIN   5
#define RELAY_PIN 18
const int RELAY_ON = HIGH, RELAY_OFF = LOW;   // jika relay terbalik, tukar nilainya
const unsigned long EMPTY_TIMEOUT = 20000;    // 20 detik tanpa gerakan = ruangan kosong (demo)

// ====== VARIABEL ======
WiFiClient net;
PubSubClient mqtt(net);
DHT dht(DHT_PIN, DHT22);

float setpoint = 25, suhu = NAN, lembap = NAN;
bool autoMode = true, acOn = false, ada = false;
unsigned long lastMotion, lastRead = 0, onSec = 0, savedSec = 0;

String T(const char* k) { return String(ID) + "/" + k; }

void pub(const char* k, String v) { mqtt.publish(T(k).c_str(), v.c_str(), true); }

void publishAll(bool occupied) {
  pub("suhu", String(suhu, 1));
  pub("lembap", String(lembap, 0));
  pub("orang", occupied ? "1" : "0");
  pub("ac", acOn ? "ON" : "OFF");
  pub("mode", autoMode ? "AUTO" : "MANUAL");
  pub("setpoint", String(setpoint, 0));
  pub("aktif", String(onSec));
  pub("hemat", String(savedSec));
}

void setAC(bool on) {
  acOn = on;
  digitalWrite(RELAY_PIN, on ? RELAY_ON : RELAY_OFF);
}

void onMessage(char* topic, byte* p, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)p[i];
  msg.trim();
  String t = topic;
  if (t == T("mode")) autoMode = (msg == "AUTO");
  else if (t == T("cmd") && !autoMode) setAC(msg == "ON");
  else if (t == T("setpoint")) setpoint = constrain(msg.toFloat(), 18, 30);
  Serial.println(t + " = " + msg);
}

void connectMqtt() {
  while (!mqtt.connected()) {
    String cid = String(ID) + "-esp-" + String(random(0xffff), HEX);
    if (mqtt.connect(cid.c_str())) {
      mqtt.subscribe(T("mode").c_str());
      mqtt.subscribe(T("cmd").c_str());
      mqtt.subscribe(T("setpoint").c_str());
      Serial.println("MQTT terhubung");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  setAC(false);
  dht.begin();
  lastMotion = millis() - EMPTY_TIMEOUT;   // awal: ruangan dianggap kosong

  WiFi.begin(WIFI_SSID, "", 6);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println(" OK");

  mqtt.setServer(MQTT_HOST, 1883);
  mqtt.setCallback(onMessage);
}

void loop() {
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();

  unsigned long now = millis();
  if (digitalRead(PIR_PIN)) lastMotion = now;
  bool occupied = (now - lastMotion) < EMPTY_TIMEOUT;

  if (now - lastRead >= 2000) {
    lastRead = now;
    float t = dht.readTemperature(), h = dht.readHumidity();
    if (!isnan(t)) { suhu = t; lembap = h; }

    if (autoMode && !isnan(suhu)) {
      if (occupied && suhu >= setpoint) setAC(true);
      else if (!occupied || suhu <= setpoint - 1) setAC(false);
      // hitung waktu AC "seharusnya nyala" tapi ruangan kosong = energi dihemat
      if (!occupied && suhu >= setpoint) savedSec += 2;
    }
    if (acOn) onSec += 2;

    publishAll(occupied);
    Serial.printf("Suhu %.1f C | Lembap %.0f%% | Orang %d | AC %s | %s\n",
                  suhu, lembap, occupied, acOn ? "ON" : "OFF", autoMode ? "AUTO" : "MANUAL");
  }
}
