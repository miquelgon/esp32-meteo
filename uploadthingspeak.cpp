#include <HTTPClient.h>
#include "uploadthingspeak.hpp"

int uploadDataHTTP(String serverName, 
                   String apiKey,
                   UploadData * ud) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKey + 
                            "&field1=" + String(ud->temperature) +    
                            "&field2=" + String(ud->humidity) +    
                            "&field3=" + String(ud->pressure);
    
    int httpResponseCode = http.POST(httpRequestData);
    http.end();
    
    Serial.println("sent:" + httpRequestData);
    Serial.println("recv:" + String(httpResponseCode));
    return 1;
}