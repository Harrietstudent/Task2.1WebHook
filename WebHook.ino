#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFiNINA.h>

#include "secrets.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   

// Create instance of type DHT:
DHT dht(DHTPIN, DHTTYPE);

// Create instance of type WiFiClient:
WiFiClient  client;

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password

unsigned long channelNumber = SECRET_CH_ID;
const char * writeAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  ThingSpeak.begin(client); 
}

void loop() {
  int statusCode = 0;

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  ThingSpeak.setField(1,t);

  ThingSpeak.setField(2,h);
  
  statusCode = ThingSpeak.writeFields(channelNumber, writeAPIKey);

  if(statusCode == 200) {
    Serial.println("statusOK, 200");
  }
  else{
    Serial.println("Problem writing to channel. HTTP error code " + String(statusCode)); 
  }
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println();
  

  // Note: ThinkSpeak only accepts a reading ever 15 seconds.
  // As per task outline, "On Arduino IDE, update your code to 
  // publish your variable’s data every 60 seconds":
  delay(60000);
}
