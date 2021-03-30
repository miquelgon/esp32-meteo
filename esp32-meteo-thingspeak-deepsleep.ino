#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <HTTPClient.h>

#include "version.h"
#include "config.h"

const char* ssid     = CFG_SSID;
const char* password = CFG_PASSWORD;
String apiKey = CFG_THINGSPEAK_APIKEY;     
const char* serverName = CFG_THINGSPEAK_SERVER;

// upload data freq in seconds
const int freq_s = 300;

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define uS_TO_mS_FACTOR 1000ULL  /* Conversion factor for micro seconds to milliseconds */

RTC_DATA_ATTR int bootCount = 0;

Adafruit_BME280 bme;

int wifiConnect() {
  Serial.println("Connecting to Wifi... ssid:" + String(ssid));
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  int wifi_retries = 0; 
  while (WiFi.status() != WL_CONNECTED) {
    if (wifi_retries > 15) {
      Serial.print("Error connecting to wifi (max_retries exceeded)");
      return 0;
    }
    delay(1000);
    Serial.print(".");
    wifi_retries ++;
        
  }
  Serial.println("WiFi connected !!!!");
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI:");
  Serial.println(WiFi.RSSI());
  return 1;
}

void wifiDisconnect() {
  WiFi.disconnect();
}

void setup() {

  long t0 = millis();
 
  Serial.begin(115200);

  Serial.println("--------------INIT setup, version: " + String(CODEVERSION));

  // get bme280 sensor data -------------------------------------------------------
  bme.begin(0x76);   
  Serial.println("BME ID: " + String(bme.sensorID()));
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  Serial.println("BME280 data: t:" + String(temperature) + " h:" + String(humidity) + " p:" + String(pressure));

  // connect to wifi & send data to server  ---------------------------------------

  if (wifiConnect() == 1) {

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKey + 
                             "&field1=" + String(temperature) +    
                             "&field2=" + String(humidity) +    
                             "&field3=" + String(pressure);
    
    int httpResponseCode = http.POST(httpRequestData);
    http.end();
    
    Serial.println("sent:" + httpRequestData);
    Serial.println("recv:" + String(httpResponseCode));
    
    wifiDisconnect();
  }

  // DEEP sleep  ---------------------------------------------
  long tx = millis() - t0;
  Serial.println("total delay ms:" + String(tx));
  
  long sleepms = 1000;
  if (freq_s * 1000 - tx > 0) {
    sleepms = freq_s * 1000 - tx;
  }
 
  esp_sleep_enable_timer_wakeup(sleepms * uS_TO_mS_FACTOR);
  Serial.println("Deep sleep for " + String(sleepms) +  " MilliSeconds");
  Serial.flush(); 
  esp_deep_sleep_start();
  
}

void loop() {
  //pass
}
