#include "M5StickCPlus.h"

#include <FastLED.h>

#include "secrets.h"

#include <WiFi.h>
#include <WiFiUdp.h>

const int frameSize = 9216;
// const int frameSize = 19200;

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
unsigned long lastConnectionTime = 0;              // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5L * 1000L; // delay between updates, in milliseconds

// char receiveBuffer[256]; //buffer to hold incoming packet
uint8_t receiveBuffer[frameSize]; //buffer to hold incoming packet
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

  if (client.connect(destinationIp, destinationReceivePort) == 1) {
    Serial.println("connected to server");
    client.println("\n\n");
    delay(200);
    if (client.read(receiveBuffer, frameSize) != 0){
      Serial.println(receiveBuffer[25]);
      Serial.println("Hi");
    }
  }

  for (int i = 0; i < frameSize; i++) {
    int coord[2];
    coord[0] = i % 96; //160 is referring to width
    coord[1] = i / 96;

    if (receiveBuffer[i] == 1) {
      M5.Lcd.drawPixel(coord[0], coord[1], WHITE);
      // Serial.println("Coordinates" + String(coord[0]) + ", " + String(coord[1]));
    }
    else {
      M5.Lcd.drawPixel(coord[0], coord[1], BLACK);
    }
  }

  client.stop();

  // if (millis() - lastConnectionTime > postingInterval) {
  //   imageBufferRequest();
  // }


}

void imageBufferRequest() {
  client.stop();

  if (client.connect(destinationIp, destinationReceivePort)) {
    Serial.println("connected to server");
    client.println('\n');

    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
