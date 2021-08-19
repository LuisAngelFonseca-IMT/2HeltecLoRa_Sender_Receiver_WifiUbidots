/*****************************************
Name: LoRa Receiver Heltec Wifi LoRa 32(2) for publishing in Ubidots 
Develop by: Luis Angel Fonseca
Date: 19/08/2021
Reference:https://github.com/jotathebest/
*****************************************/

// Include Libraries
#include "UbidotsEsp32Mqtt.h"
#include "heltec.h" 

// Define WIFI KEYs & Ubidots Keys, enter your credentials here..
const char *UBIDOTS_TOKEN = "Your token here";  
const char *WIFI_SSID = "WIFI Name";     
const char *WIFI_PASS = "WIFI password";    
const char *DEVICE_LABEL = "Device label";
const char *VARIABLE_LABEL = "Variable label"; 

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);

// Lora variables 
#define BAND    915E6  // Frecuency band: 868E6,915E6
String rssi = "RSSI --";
String packSize = "--";
String packet ;

// Ubidots callback function 
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Lora packet handle function 
void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

// Oled display data
void LoRaData(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0 , 13 , "Reci.: "+ packSize + " bytes");
  Heltec.display->drawString(0, 0, rssi);
  Heltec.display->setFont(ArialMT_Plain_24);  
  Heltec.display->drawStringMaxWidth(0 , 38 , 128, packet + " ºC");
  Heltec.display->display();
}

// SETUP LOOP
void setup()
{
  // Start serial, connect to wifi & connect to ubidots
  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();

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
  Heltec.display->drawString(0, 20, "Heltec LoRa 32 Ready!");
  Heltec.display->drawString(0, 30, "Waiting for data.....");
  Heltec.display->display();
  delay(2000);
  LoRa.receive();
}

// LOOP
void loop()
{
  // Check connection with Ubidots
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }

  // Refresh data from LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
  delay(10);

  // Publish every frecuency time
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) 
  {
    float value = packet.toFloat();
    ubidots.add(VARIABLE_LABEL, value); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0 , 26 , "Send to Ubidots!");
    Heltec.display->display();
    delay(2000);
    timer = millis();
  }
  ubidots.loop();
}
