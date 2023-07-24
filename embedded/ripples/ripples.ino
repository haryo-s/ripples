#include "M5StickCPlus.h"

#include "secrets.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <MicroOscUdp.h>

const char* ssid       = SECRET_SSID;
const char* password   = SECRET_PASS;

int button_last_value = 0;
int button_cur_value  = 0;

WiFiUDP myUdp;
IPAddress mySendIp(192, 168, 178, 21);
unsigned int mySendPort = 7777;
unsigned int myReceivePort = 8888;

IPAddress destinationIp(192, 168, 178, 10);
unsigned int destinationReceivePort = 8000;

MicroOscUdp<1024> myOsc(&myUdp, mySendIp, mySendPort);

void setup() {
  M5.begin(true, true, true);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  myUdp.begin(myReceivePort);
  myOsc.setDestination(destinationIp, destinationReceivePort);
}

void loop() {
  digitalRead(M5_BUTTON_HOME);

  if (digitalRead(M5_BUTTON_HOME) == 0) {
      myOsc.sendInt("/photo", 1);
  }

  delay(5);
}