#include "M5StickCPlus.h"

#include <FastLED.h>

#include "secrets.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <MicroOscUdp.h>



// WIFI SETTINGS

const char* ssid       = SECRET_SSID;
const char* password   = SECRET_PASS;

int button_last_value = 0;
int button_cur_value  = 0;

// UDP SETTINGS

WiFiUDP myUdp;
IPAddress mySendIp(192, 168, 178, 21);
unsigned int mySendPort = 7777;
unsigned int myReceivePort = 8888;

IPAddress destinationIp(192, 168, 178, 10);
unsigned int destinationReceivePort = 8000;

char packetBuffer[255]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";       // a string to send back

MicroOscUdp<1024> myOsc(&myUdp, mySendIp, mySendPort);

// FLASHHAT SETTINGS

#define DATA_PIN    26
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    126
#define BRIGHTNESS  25

CRGB leds[NUM_LEDS];

void setup() {
  M5.begin(true, true, true);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  myUdp.begin(myReceivePort);
  myOsc.setDestination(destinationIp, destinationReceivePort);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {

  // When home button is held down, send OSC packet
  digitalRead(M5_BUTTON_HOME);

  if (digitalRead(M5_BUTTON_HOME) == 0) {
      myOsc.sendInt("/photo", 1);
      leds[0]   = CRGB::Red;
      FastLED.show(); 
  }

    if (digitalRead(M5_BUTTON_HOME) == 1) {
      leds[0]   = CRGB::Black;
      FastLED.show(); 
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

    if (packetSize == 126) 
    {
      for (int i = 0; i < 126; i++) 
      {
        if (&packetBuffer[i] == "0") 
        {
          leds[i] = CRGB::Black;
          Serial.println("Turning off");
        }
        if (&packetBuffer[i] == "1") 
        {
          leds[i] = CRGB::Red;
          Serial.println("Turning on");
        }
      }
      FastLED.show(); 
    }
  }



  delay(5);
}