#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <LiquidCrystal.h>

//Time of seconds to update data again
#define timeSeconds 5

//LCD pin to NodeMCU
const int pin_RS = D0; 
const int pin_EN = D1; 
const int pin_d4 = D2; 
const int pin_d5 = D3; 
const int pin_d6 = D4; 
const int pin_d7 = D5; 

//FingerPrint for SSL communication, else you will get error
const uint8_t fingerprint[20] = {0x08,0x3B,0x71,0x72,0x02,0x43,0x6E,0xCA,0xED,0x42,0x86,0x93,0xBA,0x7E,0xDF,0x81,0xC4,0xBC,0x62,0x30};

ESP8266WiFiMulti WiFiMulti;
LiquidCrystal lcd(pin_RS, pin_EN, pin_d4, pin_d5, pin_d6, pin_d7);

unsigned long now = millis();
unsigned long lastTrigger = 0;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SSID", "PASSWORD");  //Change SSID,PASSWORD with your params
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Covid-19 Morocco");
  lcd.setCursor(4,1);
  lcd.print("Tracker");
  //delay(3000);
  
}

void loop() {
  now = millis();
  if((now - lastTrigger) > (timeSeconds*1000)) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    HTTPClient https;
    
    // API to get data for Morocco, you can change Morocco to your country's name
    if (https.begin(*client, "https://coronavirus-19-api.herokuapp.com/countries/morocco")) {  
      int httpCode = https.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
          char charBuf[500];
          payload.toCharArray(charBuf, 500);

          DynamicJsonDocument doc(200);
          deserializeJson(doc, payload);

          String country = doc["country"];
          String cases = doc["cases"];
          String todayCases = doc["todayCases"];
          String deaths = doc["deaths"];
          String todayDeaths = doc["todayDeaths"];
          String recovered = doc["recovered"];
          
          Serial.println("country ="+country );
          Serial.println("cases ="+cases );
          Serial.println("todayCases ="+todayCases );
          Serial.println("todayDeaths ="+todayDeaths);
          Serial.println("recovered ="+recovered);
          Serial.println("deaths ="+deaths);

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Total Cas:");
          lcd.setCursor(7,1);
          lcd.print(cases);
          delay(3000);
          lcd.clear();
          
          lcd.setCursor(0,0);
          lcd.print("Cas Aujourd'hui:");
          lcd.setCursor(7,1);
          lcd.print(todayCases); 
          delay(3000);        
          lcd.clear();
          
          lcd.setCursor(0,0);
          lcd.print("Cas Deces:");
          lcd.setCursor(7,1);
          lcd.print(todayDeaths); 
          delay(3000);
          lcd.clear();
          
          lcd.setCursor(0,0);
          lcd.print("Total Cas Deces:");
          lcd.setCursor(7,1);
          lcd.print(deaths); 
          delay(3000);
          lcd.clear();

          lcd.setCursor(0,0);
          lcd.print("Cas Retablis:");
          lcd.setCursor(7,1);
          lcd.print(recovered); 
          delay(3000);
          //lcd.clear();
          
        }
      } 
      https.end();
       } 
     }
  lastTrigger = millis();
   }
}
