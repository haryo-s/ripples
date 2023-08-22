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
IPAddress mySendIp(192, 168, 178, 21);
unsigned int mySendPort = 7777;
unsigned int myReceivePort = 8888;

IPAddress destinationIp(192, 168, 178, 24);
unsigned int destinationReceivePort = 9999;

WiFiClient client;

// char receiveBuffer[256]; //buffer to hold incoming packet
uint8_t receiveBuffer[76800]; //buffer to hold incoming packet
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
      M5.Lcd.fillScreen(RED);
      delay(500);
      // Serial.print(".");
  }

  Serial.println("Connected to wifi");
  Serial.println("\nStarting connection...");
  // if you get a connection, report back via serial:




  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  M5.Lcd.fillScreen(GREEN);
  delay(2000);
  M5.Lcd.fillScreen(BLACK);
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

  if (client.connect(destinationIp, destinationReceivePort)) {
    Serial.println("connected");

    Serial.println("Sending request");
    client.println("\n\n");
    client.read(receiveBuffer, 76800);

    client.stop(); // Close connection after sending request
    delay(1000);
  }

  for (int i = 0; i < 76800; i++) {
    
  }


}