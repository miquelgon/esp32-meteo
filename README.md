# esp32-meteo-thingspeak-deepsleep

Meteo statio based on ESP32 + BME280 sensor for temperature, humidity and barometer.

Connect to wifi + uploads sensor data to MQTT , with deepsleep.

Copy config.h.template -> config.h and enter private and config data.

MQTT topic (thingspeak compatible)
"channels/<channelID>/publish/<channelApiKey>"

-----------------------------------------------
Board Connections ESP32 - BME280: 
- esp32 3V3 - bme280 VIN
- esp32 GND - bme280 GND
- esp32 G22 - bme280 SCL
- esp32 G21 - bme280 SDA


------------------------------------------------
To set user/passwords in Mosquitto MQTT server:

1) Shell to container: docker exec -it hiot_mqtt_1 /bin/sh
2) Edit /mosquitto/config/mosquitto.passwd
"
user1:pass1
user2:pass2
"
3) Execute: mosquitto_passwd -U mosquitto.passwd

Test: 
mosquitto_sub -t "channels/#" -u user1 -P pass1

-----------------------------------------------
