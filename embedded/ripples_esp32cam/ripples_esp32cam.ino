#define  CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "secrets.h"
#include "camera_pins.h"
#include <WiFi.h>
/*
Notes for next phase with 2 ESP32CAM boards

For performance and stability reasons, it might be interesting to use a ping-pong model:

At setup both grab a frame and save it to prevFrameBuffer

CAM_A sends request to CAM_B
CAM_B takes a new frame, compares it to prevFrameBuffer and saves the result to differenceBuffer
CAM_B sends it to CAM_A
CAM_A receives it, does it stuff and sends a packet to CAM_B that it's finished
CAM_B, on receiving the finished signal, then sends a request to CAM_A
CAM_A takes a new frame, compares it to prevFrameBuffer and saves the result to differenceBuffer
CAM_A sends it to CAM_B
CAM_B receives it, does it stuff and sends a packet to CAM_A that it's finished
Repeat
*/

// CAMERA
// Frame size is 160x120 or 19200b or 19.2kb
// Frame size is 96*96 or 9216 or 9.216kb
camera_fb_t* fb;

const int frameSize = 9216;
// const int frameSize = 19200;
// const int frameSize = 76800;

const int differenceThreshold = 128;

uint8_t* prevFrameBuffer;
uint8_t* differenceBuffer;

// TCP Settings
WiFiServer server(9999);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println(ets_get_cpu_frequency());

  // Allocating framebuffers to PSRAM
  prevFrameBuffer = (uint8_t*) ps_malloc (frameSize * sizeof (uint8_t));
  differenceBuffer = (uint8_t*) ps_malloc (frameSize * sizeof (uint8_t));

  // Connecting to WiFi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to wifi. My address:");
  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress);

  // Starting TCP server
  server.begin();
  
  // Starting camera
  init_camera();

  // On startup, we grab a first frame and set that as the prevFrameBuffer
  fb = esp_camera_fb_get();
  if(!prevFrameBuffer) {
    Serial.println("Camera capture failed");
    return;
  }
  else {
    memcpy(prevFrameBuffer, fb->buf, fb->len);
    Serial.println("Previous frame buffer saved");
  }
  esp_camera_fb_return(fb);

}

void loop() {  

  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Camera capture failed");
    return;
  }
  else {
    setDifferenceBuffer(fb);
    // Then, if the length is the same, copy from the frameBuffer to prevFrameBuffer
    // One issue might be in the length is not crrect
    if (fb->len == frameSize) {
      Serial.println("Previous frame buffer updated");
      memcpy(prevFrameBuffer, fb->buf, fb->len);
    }
  }
  esp_camera_fb_return(fb);

  // TODO: This WiFi transfer might be done better with an async WebServer
  WiFiClient client = server.available();
  if (client && differenceBuffer) {
    Serial.println("New Client.");        // print a message out the serial port
    char c = client.read();               // read a byte, then
    if (client.available()) {             // if there's bytes to read from the client,
      Serial.write(c);                    // print it out the serial monitor
      if (c == '\n') {                    // if the byte is a newline character
        Serial.println("Request received. Replying with differenceBuffer");
        // client.write(prevFrameBuffer, frameSize);
        client.write(differenceBuffer, frameSize);
        Serial.println(String(differenceBuffer[25]));
      }
    }
  }

  // delay(40);

}

//TODO: This could return a bool for success or failure
void setDifferenceBuffer(camera_fb_t* frameBuffer) {
  // First we check if prevFrameBuffer is valid. 
  if(prevFrameBuffer) { 
    for (int i = 0; i < frameSize; i++) {
      uint8_t difference = abs(frameBuffer->buf[i] - prevFrameBuffer[i]);
      differenceBuffer[i] = (difference > differenceThreshold) ? 1 : 0;
    }
  }
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
  config.frame_size   = FRAMESIZE_96X96;   // 96x96
  // config.frame_size   = FRAMESIZE_QQVGA;   // 160x120
  // config.frame_size   = FRAMESIZE_QVGA; // 320x240
  config.pixel_format = PIXFORMAT_GRAYSCALE; // 1 byte per pixel, 0-255
  // config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 2;

  return esp_camera_init(&config);
}