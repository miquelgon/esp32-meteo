# esp32-meteo-thingspeak-deepsleep

Meteo statio based on BME280 sensor, for temperature, humidity and barometer.

Connect to wifi + uploads data to thingspeak server every 5 min, with deepsleep.

Enter private data in config.h  (Wifi SSID & password + thingspeak write API KEY)

-------------------------
Connections: 
- esp32 3V3 - bme280 VIN
- esp32 GND - bme280 GND
- esp32 G22 - bme280 SCL
- esp32 G21 - bme280 SDA







