struct UploadData{
  float temperature;
  float humidity;
  float pressure;
};

int uploadDataHTTP(String serverName, 
                   String apiKey,
                   UploadData * ud);