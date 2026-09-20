#define BLYNK_TEMPLATE_ID "TMPL33jOkQQ5E"
#define BLYNK_TEMPLATE_NAME "Weather monitoring system"
#define BLYNK_AUTH_TOKEN "B2e8vNSPYRRx0AIAH4vgLaA97oASlaay"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

char ssid[] = "Galaxy S25 Ultra";
char pass[] = "12345687";

#define DHTPIN D5 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp; 

const int rainPin = A0;

BlynkTimer timer;

void sendSensorData()
{
  Serial.println("-----------------------");

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error: Failed to read from DHT sensor!");
  } else {
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Serial.print("Temp: "); Serial.print(t); Serial.print("C  ");
    Serial.print("Hum: "); Serial.print(h); Serial.println("%");
  }

  float pressure = bmp.readPressure();
  
  if (pressure > 0) {
    pressure = pressure / 100.0;
    Blynk.virtualWrite(V3, pressure);
    Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
  } else {
    Serial.println("Error: BMP280 not connected or reading 0");
  }

  int rainRaw = analogRead(rainPin);
  int rainPercent = map(rainRaw, 1024, 0, 0, 100); 
  rainPercent = constrain(rainPercent, 0, 100); 

  Blynk.virtualWrite(V2, rainPercent);
  Serial.print("Rain: "); Serial.print(rainPercent); Serial.println("%");
}

void setup()
{
  Serial.begin(9600);
  Serial.println("\nStarting Weather Station...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  
  dht.begin();
  
  if (!bmp.begin(0x76)) {
    if (!bmp.begin(0x77)) {
      Serial.println("Error: Could not find BMP280 sensor!");
      Serial.println("Check wiring: SDA->D2, SCL->D1, VCC->3.3V");
    } else {
      Serial.println("Success: BMP280 found at 0x77");
    }
  } else {
    Serial.println("Success: BMP280 found at 0x76");
  }

  timer.setInterval(2000L, sendSensorData);
}

void loop()
{
  Blynk.run();
  timer.run();
}
