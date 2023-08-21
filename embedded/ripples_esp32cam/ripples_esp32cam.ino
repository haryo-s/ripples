#define  CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"

#include "secrets.h"
#include "camera_pins.h"

#include <WiFi.h>
// #include <MicroOscUdp.h>

// CAMERA
// Frame size is 160x120 or 19200b or 19.2kb
camera_fb_t* fb;

const int frameSize = 19200;
// uint8_t prevFrameBuffer[frameSize];
uint8_t* prevFrameBuffer;
uint8_t* differenceBuffer;

// UDP SETTINGS
IPAddress mySendIp(192, 168, 178, 24);
unsigned int mySendPort = 7777;
unsigned int myReceivePort = 8888;

IPAddress destinationIp(192, 168, 178, 255);
unsigned int destinationReceivePort = 8888;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  prevFrameBuffer = (uint8_t*) ps_malloc (frameSize * sizeof (uint8_t));
  differenceBuffer = (uint8_t*) ps_malloc (frameSize * sizeof (uint8_t));

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  init_camera();
}

void loop() {
  delay(250); // 40ms or 25fps

  fb = esp_camera_fb_get();

  if(prevFrameBuffer) {
    for (int i = 0; i < frameSize; i++) {
      uint8_t difference = abs(fb->buf[i] - prevFrameBuffer[i]);
      differenceBuffer[i] = (difference > 64) ? 1 : 0;
    }
  }

  if(!fb){
    Serial.println("Camera capture failed");
    return;
  }
  else {
    if (fb->len == frameSize) {
      memcpy(prevFrameBuffer, fb->buf, fb->len);
    }
    // myUdp.beginPacket(destinationIp, destinationReceivePort);
    // myUdp.write(differenceBuffer, frameSize);
    // myUdp.endPacket();
  }
  esp_camera_fb_return(fb);
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
  config.frame_size   = FRAMESIZE_QQVGA;   // 160x120
  // config.frame_size   = FRAMESIZE_QVGA; // 320x240
  config.pixel_format = PIXFORMAT_GRAYSCALE; // 1 byte per pixel, 0-255
  // config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  return esp_camera_init(&config);
}