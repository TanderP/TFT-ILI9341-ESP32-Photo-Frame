#include <SD.h>
#include <TJpg_Decoder.h> 
#include <FS.h>
#include <SPI.h>
#include <LittleFS.h>
#include "SPIFFS.h"
#include <vector> 
// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library




#define chipSelect 15 
#define LED 2
#define BL 4


bool init_SD = true;

std::vector<String> image_name;

void SD_INIT();

void setup() {
  Serial.begin(115200);
  Serial.print("ESP BEGIN");
  pinMode(LED,OUTPUT);
  pinMode(BL,OUTPUT);
  pinMode(chipSelect, OUTPUT);
  
  digitalWrite(chipSelect,HIGH);
  tft.fillScreen(TFT_BLACK);
  digitalWrite(BL,HIGH);
  SD_INIT();

   tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  
}


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;
  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);
  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);
  // Return 1 to decode next block
  return 1;
}


void loop() {
  File file;
  if(init_SD){
    TJpgDec.setCallback(tft_output);
     File root = SD.open("/");
  if(!root){
      Serial.println("Failed to open root directory");
      return;
  }
  file = root.openNextFile();  // Opens next file in root
  while(file)
  {
    if(!file.isDirectory())
    {
      //Serial.println(file.name());
      image_name.push_back(file.name());
      
    }
    file = root.openNextFile();  // Opens next file in root
  }

  Serial.println("Filenames stored in vector:");
  for (size_t i = 0; i < image_name.size(); i++) {
    Serial.println(image_name[i]);
  }

  root.close();
  init_SD = false;

  }else{  
      Serial.println("LOOP");
      
  for (int i = 0; i < image_name.size(); i++) {
     
  // Time recorded for test purposes
  uint32_t t = millis();

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getSdJpgSize(&w, &h, "/"+image_name[i]);
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  // Draw the image, top left at 0,0
  TJpgDec.drawSdJpg(0, 0, "/"+image_name[i]);

  // How much time did rendering take
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");

  // Wait before drawing again

    delay(5000);
  }
  
  

  } 

}

void SD_INIT(){
  if(!SD.begin(chipSelect)){
    delay(1000);
    Serial.println("Failed to init, Restarting");
    delay(2000);
    ESP.restart();
  }else{
    digitalWrite(LED,HIGH);
    Serial.println("Succed");
  }
}

