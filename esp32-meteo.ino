#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <PubSubClient.h>

#include "version.h"
#include "config.h"
#include "upload.h"

int freq_s = CFG_FREQ_S;

const char * ssid     = CFG_SSID;
const char * password = CFG_PASSWORD;

long channelID =       CFG_CHANNELID;                    
String channelApiKey = CFG_APIKEY;

char mqttServer [] =   CFG_MQTT_SERVER;
int  mqttPort =        CFG_MQTT_PORT;
char mqttUserName[] =  CFG_MQTT_USER;
char mqttPass[] =      CFG_MQTT_PASS;

#define uS_TO_mS_FACTOR 1000ULL  /* Conversion factor for micro seconds to milliseconds */

char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

Adafruit_BME280 bme;
WiFiClient wifiClient;
PubSubClient mqttClient(mqttServer, mqttPort, wifiClient);
RTC_DATA_ATTR int sendCount = 0;

int wifiConnect() {
  Serial.println("Connecting to Wifi... ssid:" + String(ssid));
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

  Serial.print("WiFi connected !!!! IP:");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI:");
  Serial.println(WiFi.RSSI());
  return 1;
}

// Generate MQTT ClientID
const char *getClientID(){
  static char clientID[9];
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    clientID[8]='\0';
  return clientID;
}

int mqttConnect() 
{
  int mqtt_retries = 0; 
  int connectok = 0;

  while (!mqttClient.connected())  {
      const char *clientID = getClientID();
      Serial.print("Attempting MQTT connection... clientID:");
      Serial.println(clientID);

      // Connect to the MQTT broker.
      if (mqttClient.connect(clientID, mqttUserName, mqttPass)) {
          Serial.println("MQTT connected OK!");
          connectok = 1;
          break;
      } else  {
          Serial.print("failed, rc=");
          // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
          Serial.print(mqttClient.state());
          Serial.println(" try again in 1 seconds");
          delay(1000);
      }

      mqtt_retries ++;
      if (mqtt_retries > 10) {
          Serial.print("Error connecting to MQTT (max_retries exceeded)");
          connectok = 0;
          break;
      }
  }
  return connectok;
  
}

void mqttPublishFeed(UploadData * ud) {
  String data = "field1=" + String(ud->temperature) + 
                "&field2=" + String(ud->humidity) + 
                "&field3=" + String(ud->pressure) +
                "&field4=" + String(ud->sendCount);
                
  int length = data.length();
  const char *msgBuffer;
  msgBuffer=data.c_str();
  
  Serial.print("msg:");
  Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString = "channels/" + String( channelID ) + "/publish/"+ channelApiKey;
  length = topicString.length();
  const char *topicBuffer;
  topicBuffer = topicString.c_str();

  Serial.print("topic:");
  Serial.println(topicBuffer);
  
  mqttClient.publish( topicBuffer, msgBuffer );
}


void wifiDisconnect() {
  WiFi.disconnect();
}


UploadData getUploadData () {
  UploadData ud; 
  ud.temperature = bme.readTemperature();
  ud.humidity = bme.readHumidity();
  ud.pressure = bme.readPressure() / 100.0F;
  ud.sendCount = sendCount;
  return ud;                   
}

void setup() {
  ++sendCount;
  long t0 = millis();
  Serial.begin(115200);
  Serial.println("--------------INIT setup, sendCount: " + String(sendCount));

  // init BME sensor
  bme.begin(0x76);   
  Serial.println("BME ID: " + String(bme.sensorID()));

  // connect to wifi & send data to server  ---------------------------------------
  if (wifiConnect() == 1) {

    int connectok = mqttConnect();
    if (connectok == 1) {
        UploadData ud = getUploadData(); 
        Serial.println("BME280 data: t:" + String(ud.temperature) + " h:" + String(ud.humidity) + 
                       " p:" + String(ud.pressure)+ " count:" + String(ud.sendCount));
        mqttPublishFeed(&ud);
    }

    // time to send message before deep sleep
    delay(1000);

    mqttClient.disconnect();
    wifiDisconnect();
    
  }

  //deep sleep
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


}
