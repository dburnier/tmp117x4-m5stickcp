/*
  Reading 4 TMP117 temperature sensor
  By: Daniel Burnier
  Date: October 21st, 2022
  License: Totally open and free. You can
  basically do whatever you want with this code.

  This example prints the temperature in C of 4 TMP117 personal board.

  Hardware Connections:
  Attach M5Stick-CPlus module to computer using a USB cable.
  Connect until 4 TMP117 Board using GROVE cable and .
  Open Serial Monitor at 115200 baud.

  RGR565 color pickers:
    https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
    https://chrishewett.com/blog/true-rgb565-colour-picker/
  
*/

#include <Arduino.h>
#include <M5StickCPlus.h>
// #include <Wire.h>
#include <secrets.h>
#include <WiFi.h>
// #include <HTTPClient.h>
#include "time.h"
#include "Streaming.h"
#include "SparkFun_TMP117.h"
// #include "esp32-hal-ledc.h"
#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux

QWIICMUX myMux;

#define SCREEN_X 240
#define SCREEN_Y 135

// #define MAX_CO2_PIX 50
// #define MIN_CO2_PPM 500
// #define MAX_CO2_PPM 2000
// #define PIX_FROM_BOT 10

// #define CO2_AVG_TIME_SECS 360
// #define TIME_SPAN_HOURS 24

#define MY_DARKGRAY 0x0020
#define MY_RED 0xD34D
#define MY_BLUE 0x5D36
#define MY_YELLOW 0xB58A

#define TMP117_ADDRESS_BASE   0x48
TMP117 TMP117_Sensor[4];

//WiFi credentials
const char* ssid       = SECRET_WIFI_SSID;
const char* password   = SECRET_WIFI_PSW;
bool netConnected = false;

// M5.Lcd cursor position
int row, col;

void setup()
{
  M5.begin();

  Serial.begin(115200);

  delay(1000);

  //Set up the display
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(TFT_ORANGE);
  M5.Lcd.setHighlightColor(RED);
  M5.Lcd.highlight(true);
  row = 0; col = 0;
  M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
  M5.Lcd.print("4 x TMP117 Seppors");
  Serial.println("4 x TMP117 Sensors");

  // Check WiFi access
  WiFi.begin(ssid, password);
  delay(1000);
  netConnected = (WiFi.status() == WL_CONNECTED);
  row = 2; col = 2;
  M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
  Serial.print("WiFi ");
  Serial.println(netConnected ? "connected" : "not connected");
  M5.Lcd.print("WiFi ");
  M5.Lcd.print(netConnected ? "connected" : "not connected");

  Wire.begin();
  row = 3; col = 2;
  M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
  if (myMux.begin() == false)
  {
    Serial.println("Mux not detected. Freezing...");
    M5.Lcd.print("Mux not detected. Freezing...");
    // while (1)
      delay(10);
  }
  Serial.println("\nMux detected\n");
  M5.Lcd.print("Mux detected");

  myMux.setPortState(0b11110); //Connect TMP117 sensors to port labeled '1', '2', '3' and '4' on the mux. The must be all with different addresses

  for (int offset=0; offset<4; offset++)
  {
    row = 4; col = 2;
    M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
    Serial.printf("TMP117 sensor %d ",offset);
    M5.Lcd.printf("TMP117 sensor %d ",offset);
    // Serial.printf("TMP117 sensor %d not detected. Please check wiring. Freezing...\n",offset);
    if (TMP117_Sensor[offset].begin(TMP117_ADDRESS_BASE + offset) == false)
    {
      Serial.printf("not detected. Please check wiring. Freezing...",offset);
      M5.Lcd.printf("not detected. Please check wiring. Freezing...",offset);
      // Serial << "TMP117 sensor " << offset << " not detected. Please check wiring. Freezing...\n";
      // while(1)
      {delay(1);}
    }
    else
    {
      Serial.printf("detected");
      M5.Lcd.printf("detected");
    }
    delay(1000);
  }

      while(1)
      {delay(1);}
  delay(1000);
  M5.Lcd.fillScreen(TFT_BLACK);

  //The TMP117_Sensor has data ready every two seconds

  row = 2; col = 0;
  M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
  M5.Lcd.println(" T [°C]  #0  xx.xx  "); // Row 2
  M5.Lcd.println("         #1  xx.xx  "); // Row 3
  M5.Lcd.println("         #2  xx.xx  "); // Row 4
  M5.Lcd.println("         #3  xx.xx  "); // Row 5
}

void loop()
{
  M5.update();

  row = 2;

  Serial.print("T  [°C]");
  for (int offset=0; offset<4; offset++)
  {
    col = 13;
    M5.Lcd.setCursor(col * M5.Lcd.textsize * 6, row * M5.Lcd.textsize * 8);
    if (TMP117_Sensor[offset].dataReady())
    {
      Serial.printf(" %2.2f",TMP117_Sensor[offset].readTempC());
      M5.Lcd.printf("%2.2f",TMP117_Sensor[offset].readTempC());
      // Serial << " " << TMP117_Sensor[offset].readTempC();
    }
    else
    {
      Serial.print(" --.--");
      M5.Lcd.printf("--.--");
    }
    row++;
  }
  Serial.print("\n");

  // String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"temp=" + String(t2);
  // Serial.print("POST data to spreadsheet:");
  // Serial.println(urlFinal);
  // HTTPClient http;
  // http.begin(urlFinal.c_str());
  // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  // int httpCode = http.GET(); 
  // Serial.print("HTTP Status Code: ");
  // Serial.println(httpCode);
  //---------------------------------------------------------------------
  //getting response from google sheet
  // String payload;
  // if (httpCode > 0) {
  //     payload = http.getString();
  //     Serial.println("Payload: "+payload);    
  // }
  //---------------------------------------------------------------------
  // http.end();

  delay(2000);
}