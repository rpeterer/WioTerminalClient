#include "TFT_eSPI.h"
#include "LIS3DHTR.h"
#include "rpcWiFi.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "config.h"


// function declarations
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void printStatusLine(const char* msg);
void updateScreen(char * new_msg);
void playSound();
void publishAcceleration();
void publishLight();

// global variables
TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
unsigned long currentTime;
String privateTopic = "msg/" + String(MQTT_CLIENT_ID);
String publicTopic = "msg/0";

void setup() {
  // setup serial
  Serial.begin(115200);

  // setup tft
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(2);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  // setup accelerometer
  lis.begin(Wire1);
  if (!lis) {
    while (1)
      ;
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G);

  // setup wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  int i = 0;
  const char msg[3][4] = {".", "..", "..."};
  printStatusLine(msg[i++ % 3]);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    printStatusLine(msg[i++ % 3]);
    WiFi.begin(SSID, PASSWORD);
  }
  printStatusLine(WiFi.localIP().toString().c_str());

  // setup mqtt
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  // setup buzzer
  pinMode(WIO_BUZZER, OUTPUT);

  // setup light sensor
  pinMode(WIO_LIGHT, INPUT);

  // setup cyclic counter
  currentTime = millis();
 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (millis() - currentTime >= CYCLE_TIME_MS) {
    currentTime = millis();

    // cyclic loop
    publishAcceleration();
    publishLight();
  }
  client.loop();
}

void playSound() {
  analogWrite(WIO_BUZZER, 128);
  delay(100);
  analogWrite(WIO_BUZZER, 0);
}

void publishLight() {
  String lightTopic = "tele/" + String(MQTT_CLIENT_ID) + "/light";
  String lightValue = String(analogRead(WIO_LIGHT));
  if (!client.publish(lightTopic.c_str(), lightValue.c_str())) {
    Serial.println("Light message failed to send.");
  }
}

void publishAcceleration() {
  float x, y, z;

  x = lis.getAccelerationX();
  y = lis.getAccelerationY();
  z = lis.getAccelerationZ();

  String accTopic = "tele/" + String(MQTT_CLIENT_ID) + "/acc";
  String data = "{\"x\": " + String(x) + "," + "\"y\": " + String(y) + "," + "\"z\": " + String(z) + "}";    
  if (!client.publish(accTopic.c_str(), data.c_str())) {
    Serial.println("Acc message failed to send.");
  }
}


void printStatusLine(const char* msg) {
  Serial.println(msg);
  char buffer[25];
  sprintf(buffer, "ID:%-2.2s IP:%-15.15s", MQTT_CLIENT_ID, msg);
  tft.drawString(buffer, 5, 10);
  tft.drawLine(0, 40, 320, 40, TFT_BLACK);
}

void updateScreen(char * new_msg) {
  const int ROW_NR = 6;
  static int head = 0;
  static char lines[ROW_NR][30] = {'\0', '\0', '\0', '\0', '\0', '\0'};

  strcpy(lines[head % ROW_NR], new_msg);
  for (int i = 0; i < ROW_NR; i++) {
    tft.drawString(lines[(ROW_NR + head - i) % ROW_NR], 5, 50 + i * 30);
  }

  head++;
}

void callback(char* topic, byte* payload, unsigned int length) {
  char* message = {reinterpret_cast<char*>(payload)};
  message[length] = '\0';
  char buffer[30];
  sprintf(buffer, ">%-26.26s", message);
  
  if (!strcmp(topic, publicTopic.c_str())) {
    updateScreen(buffer);
    playSound();
  } else if (!strcmp(topic, privateTopic.c_str())) {
    updateScreen(buffer);
    playSound();
  } else {
    Serial.println(message);
  }
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (client.connect(MQTT_CLIENT_NAME)) {
      Serial.println("connected");

      // subscribe to topic
      client.subscribe(publicTopic.c_str());
      client.subscribe(privateTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}