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

//-------------------------------------------
// Constants
//
// constexpr := "This is a compile time constant"
// static := "This symbol is only for this .cpp file"
//-------------------------------------------
static const constexpr int FRAME_SIZE = 96 * 96;
static const constexpr int DIFFERENCE_THRESHOLD = 128;

//-------------------------------------------
// Global data
//
// (I like to put stuff in structs to qualify 
//  names.  This is making one instance of a struct with
//  these fields that is static [only accessible from this .cpp])
//-------------------------------------------
static struct 
{
  // Previous frame we got from the camera.  
  // null if we haven't ever gotten one
  camera_fb_t* previousFrame = nullptr;

  // FRAME_SIZE long array of 0/1 values, representing whether that pixel
  // was different enough, allocated in PSRAM
  //
  // (You could also do this without a heap via the appropriate linker
  // instruction to tell it to place a plain `uint8_t differenceBuffer[FRAME_SIZE]`,
  // or better `std::array<uint8_t, FRAME_SIZE>` (if you have C++11) in PSRAM)
  //
  // But a heap allocation is fine too.
  uint8_t* differenceBuffer = nullptr;
} globals;

// TCP Settings
WiFiServer server(9999);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Allocate space for differences in PSRAM & zero it. The zeroing is to make it have
  // non-trash values before we have two good camera frames and write it normally
  globals.differenceBuffer = (uint8_t*)ps_malloc(FRAME_SIZE * sizeof(uint8_t));
  memset(globals.differenceBuffer, 0, FRAME_SIZE * sizeof(uint8_t));

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
}

void loop() {  
  // Acquire a new frame
  camera_fb_t* newFrame = esp_camera_fb_get();
  if (newFrame == nullptr)
  {
    // esp32-camera returns nullptr when camera access times out.
    // Or if there were no more buffers left, but that shouldn't happen
    Serial.println("Camera capture failed");
    return;
  }

  // If we don't have a previous frame, there's no differences to compute.
  // This frame becomes our previous frame and we're done
  if (globals.previousFrame == nullptr)
  {
    globals.previousFrame = newFrame;
    return;
  }

  // Compute the difference
  for (auto pxNum = 0; pxNum < FRAME_SIZE; ++pxNum)
  {
    // Doing this in wider signed math probably isn't any slower than doing it 
    // with conditionals
    const auto newPx = (int)newFrame->buf[pxNum];
    const auto oldPx = (int)globals.previousFrame->buf[pxNum];
    const auto delta = newPx - oldPx;
    const auto absDelta = std::abs(delta);
    globals.differenceBuffer[pxNum] = (uint8_t)((absDelta > DIFFERENCE_THRESHOLD) ? 1u : 0u);
  }

  // Old frame -> return to camera
  // New frame -> old frame
  // We're only shuffling pointers around, not copying array content
  esp_camera_fb_return(globals.previousFrame);
  globals.previousFrame = newFrame;

  // TODO: This WiFi transfer might be done better with an async WebServer
  // WiFiClient client = server.available();
  // if (client && differenceBuffer) {
  //   Serial.println("New Client.");        // print a message out the serial port
  //   char c = client.read();               // read a byte, then
  //   if (client.available()) {             // if there's bytes to read from the client,
  //     Serial.write(c);                    // print it out the serial monitor
  //     if (c == '\n') {                    // if the byte is a newline character
  //       Serial.println("Request received. Replying with differenceBuffer");
  //       // client.write(prevFrameBuffer, frameSize);
  //       client.write(differenceBuffer, frameSize);
  //       Serial.println(String(differenceBuffer[25]));
  //     }
  //   }
  // }
  Serial.println("Loop");

}

// //TODO: This could return a bool for success or failure
// void setDifferenceBuffer(camera_fb_t* frameBuffer) {
//   // First we check if prevFrameBuffer is valid. 
//   if(prevFrameBuffer) { 
//     for (int i = 0; i < frameSize; i++) {
//       uint8_t difference = abs(frameBuffer->buf[i] - prevFrameBuffer[i]);
//       differenceBuffer[i] = (difference > differenceThreshold) ? 1 : 0;
//     }
//   }
// }

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