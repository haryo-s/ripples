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

char packetBuffer[255]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";       // a string to send back

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

  // When home button is held down, send OSC packet
  digitalRead(M5_BUTTON_HOME);

  if (digitalRead(M5_BUTTON_HOME) == 0) {
      myOsc.sendInt("/photo", 1);
  }

  // Now we're going to see if we can receive something
  int packetSize = myUdp.parsePacket();

  if (packetSize) {

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");

    IPAddress remoteIp = myUdp.remoteIP();

    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(myUdp.remotePort());

    // read the packet into packetBufffer

    int len = myUdp.read(packetBuffer, 255);

    if (len > 0) {

      packetBuffer[len] = 0;
    }

    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received

    // myUdp.beginPacket(myUdp.remoteIP(), myUdp.remotePort());
    // myUdp.write(ReplyBuffer);
    // myUdp.endPacket();

  }

  delay(5);
}