#define  CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"

#include "secrets.h"
#include "camera_pins.h"

#include <WiFi.h>
#include <WiFiUdp.h>
// #include <MicroOscUdp.h>

// CAMERA
int FRAMESIZE = FRAMESIZE_QVGA; //320x240 or 76800b or 76.8kb
camera_fb_t* fb;

// UDP SETTINGS
WiFiUDP myUdp;
IPAddress mySendIp(192, 168, 178, 24);
unsigned int mySendPort = 7777;
unsigned int myReceivePort = 8888;

IPAddress destinationIp(192, 168, 178, 255);
unsigned int destinationReceivePort = 8888;

char sendBuffer[76800];

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  init_camera();
}

void loop() {
  delay(100);

  fb = esp_camera_fb_get();

  if(!fb){
    Serial.println("Camera capture failed");
    return;
  }
  else {
    // sendBuffer = fb->buf;
    myUdp.beginPacket(destinationIp, destinationReceivePort);
    myUdp.write(fb->buf, fb->len);
    myUdp.endPacket();
    // Serial.println(fb->buf[0]);
    Serial.println(fb->len);
  }
  esp_camera_fb_return(fb);

  delay(100);
}

esp_err_t init_camera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size   = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_GRAYSCALE; // 1 byte per pixel, 0-255
  // config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  return esp_camera_init(&config);
}