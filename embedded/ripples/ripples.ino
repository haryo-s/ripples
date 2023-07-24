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
unsigned int myReceivePort = 8888;
IPAddress mySendIp(192, 168, 178, 21);
unsigned int mySendPort = 7777;

IPAddress destinationIp(192, 168, 178, 10);
unsigned int destinationPort = 8000;

MicroOscUdp<1024> myOsc(&myUdp, mySendIp, mySendPort);

void setup() {
  M5.begin(true, true, true);

  pinMode(39, INPUT);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");

  myUdp.begin(myReceivePort);
  myOsc.setDestination(destinationIp, destinationPort);

}

void loop() {
  digitalRead(M5_BUTTON_HOME);

  if (digitalRead(M5_BUTTON_HOME) == 0) {
    M5.Lcd.fillRect(20, 25, 75, 85,
                    BLACK);  // Draw a black rectangle 75 by 85 at (20,25).
                              // 在(20,25)处绘制宽75,高85的黑色矩形
    if (button_cur_value == 0) {
      M5.Lcd.setCursor(40, 25);
      M5.Lcd.print("0");  // display the status.  在屏幕上显示按键的状态
      M5.Lcd.setCursor(40, 45);
      M5.Lcd.print("pre");

      myOsc.sendInt("/photo", 1);
    } 
    else {
      M5.Lcd.setCursor(40, 25);
      M5.Lcd.print("1");  // display the status
      M5.Lcd.setCursor(40, 45);
      M5.Lcd.print("rel");
    }
    // button_last_value = button_cur_value;
  }

  delay(5);
}