#include "M5StickCPlus.h"

#include <FastLED.h>

#include "secrets.h"

#include <WiFi.h>
#include <WiFiUdp.h>

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

char receiveBuffer[256]; //buffer to hold incoming packet
// char receiveBuffer[76800]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";       // a string to send back

// FLASHHAT SETTINGS

#define DATA_PIN    26
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define BRIGHTNESS  10

const int LED_HOR = 18;
const int LED_VER = 7;
const int NUM_LEDS = LED_HOR * LED_VER; 

CRGB leds[NUM_LEDS];

void setup() {
  M5.begin(true, true, true);
  pinMode(M5_LED, OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  myUdp.begin(myReceivePort);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // When home button is held down, send OSC packet
  digitalRead(M5_BUTTON_HOME);

  if (digitalRead(M5_BUTTON_HOME) == 0) {
    digitalWrite (M5_LED, LOW);
    for (int i = 0; i < NUM_LEDS; i++) 
    {
      leds[i] = CRGB::Black;
    }
    Serial.println("Resetting array");
    FastLED.show(); 
  }

  if (digitalRead(M5_BUTTON_HOME) == 1) {
    digitalWrite(M5_LED, HIGH);
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

    // read the packet into receiveBuffer
    int len = myUdp.read(receiveBuffer, packetSize);

    if (len > 0) {
      receiveBuffer[len] = 0;
    }

    int receiveBufferIntArray[packetSize];

    for (int i = 0; i < packetSize; i++) {
      receiveBufferIntArray[i] = (int)(receiveBuffer[i]);
    }

    Serial.println(receiveBufferIntArray[0]);
    Serial.println(receiveBufferIntArray[255]);

    // Serial.println("Contents:");
    // Serial.println(receiveBuffer);

    if (packetSize == 256) 
    {
      for (int i = 0; i < 126; i++) 
      {
        if (receiveBufferIntArray[i] <= 128) 
        {
          leds[i] = CRGB::Black;
          // Serial.println("Turning off");
        }
        if (receiveBufferIntArray[i] >= 127) 
        {
          leds[i] = CRGB::Red;
          // Serial.println("Turning on");
        }
      }
      FastLED.show(); 
    }
  }
  // delay(100);
}