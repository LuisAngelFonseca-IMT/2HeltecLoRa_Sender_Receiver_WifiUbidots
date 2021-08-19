/*****************************************
Name: LoRa Sender Heltec Wifi LoRa 32(2) 
Develop by: Luis Angel Fonseca
Date: 19/08/2021
Reference: Biblioman, search in youtube
*****************************************/

// Include Libraries
//#include <stdlib.h>
#include "heltec.h"
#include "images.h"
#include "DHT.h"

// Define constants
#define DHTPIN 13
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define BAND 915E6  // Frecuency band: 868E6,915E6

// Lora variables 
String rssi = "RSSI --";
String packSize = "--";
String packet ;

// Set PINS sensor
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  // Setup DHT Sensor
  dht.begin(); 
    
  // Configuration of Heltec Board 
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  // Oled config 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 24, "Starting.....");
  Heltec.display->display();
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 24, "Heltec LoRa 32 Ready!");
  Heltec.display->display();
  delay(2000);
}

void loop()
{
  // Read sensor values
  float value = dht.readTemperature(); 
  
  // Convert value to a type char array
  char valuestring[20];
  dtostrf(value, 3, 1, valuestring); // 3 = digits, 1 = decimals 

  // Crate string d, from valuestring 
  String d(valuestring);

  // Oled display
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Sending Temperature:");
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(0, 26, d +" ºC");
  Heltec.display->display();

  // Send data
  LoRa.beginPacket();
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(d);
  LoRa.endPacket();

  // Led blink 
  digitalWrite(LED, HIGH); 
  delay(1000);                       
  digitalWrite(LED, LOW);    
  delay(1000);                       
}
