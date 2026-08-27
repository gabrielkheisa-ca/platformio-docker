#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

// --- WiFi Configuration ---
#ifndef WIFI_SSID
  #define WIFI_SSID "YOUR_WIFI_SSID"
#endif
#ifndef WIFI_PASS
  #define WIFI_PASS "YOUR_WIFI_PASSWORD"
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// Non-blocking timer for LED blink
unsigned long previousMillis = 0;
const long interval = 100; // Blink interval in milliseconds (1 second)

void setup() {
  Serial.begin(115200);
  
  // Set onboard LED pin as output (Onboard LED is active LOW)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Turn LED off initially

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // --- OTA Setup ---
  ArduinoOTA.setHostname("wemos-d1-mini");
  // ArduinoOTA.setPassword("admin123"); // Optional password protection

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Update Starting...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA Listener Ready");
}

void loop() {
  // CRITICAL: Must be called frequently to process incoming OTA requests
  ArduinoOTA.handle();

  // NON-BLOCKING BLINK: Never use delay() in OTA code
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Toggle the LED state
    int currentState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !currentState);
  }
}