/*
 * Weather Station: DHT11 + Rain Sensor + BMP280
 * NO LCD VERSION
 *
 * Wiring:
 * - DHT11 Data  -> Pin D4 
 * - Rain Sensor -> Pin A0 (Analog)
 * - BMP280 SDA  -> Pin D2 (GPIO 4)
 * - BMP280 SCL  -> Pin D1 (GPIO 5)
 * - BMP280 VCC  -> 3.3V (IMPORTANT: Do not use 5V)
 * - BMP280 GND  -> GND
 */

// --- BLYNK CONFIGURATION ---
#define BLYNK_TEMPLATE_ID "TMPL33jOkQQ5E"
#define BLYNK_TEMPLATE_NAME "Weather monitoring system"
#define BLYNK_AUTH_TOKEN "B2e8vNSPYRRx0AIAH4vgLaA97oASlaay"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// --- WIFI CREDENTIALS ---
char ssid[] = "Galaxy S25 Ultra";    // Your WiFi network name
char pass[] = "12345687";   // Your WiFi password

// --- SENSOR CONFIGURATION ---

// 1. DHT Sensor
#define DHTPIN D5      // Pin D5
#define DHTTYPE DHT11  // Change to DHT22 if needed
DHT dht(DHTPIN, DHTTYPE);

// 2. BMP280 Sensor (I2C)
Adafruit_BMP280 bmp; 

// 3. Rain Sensor
const int rainPin = A0;

BlynkTimer timer;

// --- FUNCTION TO READ AND SEND DATA ---
void sendSensorData()
{
  Serial.println("-----------------------");

  // 1. Read DHT (Temp & Humidity)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error: Failed to read from DHT sensor!");
  } else {
    Blynk.virtualWrite(V0, t); // V0 for Temperature
    Blynk.virtualWrite(V1, h); // V1 for Humidity
    Serial.print("Temp: "); Serial.print(t); Serial.print("C  ");
    Serial.print("Hum: "); Serial.print(h); Serial.println("%");
  }

  // 2. Read BMP280 (Pressure)
  // We use a check to see if pressure is valid (non-zero)
  float pressure = bmp.readPressure();
  
  if (pressure > 0) {
    pressure = pressure / 100.0; // Convert Pascals to hPa
    Blynk.virtualWrite(V3, pressure); // V3 for Pressure
    Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
  } else {
    Serial.println("Error: BMP280 not connected or reading 0");
  }

  // 3. Read Rain Sensor
  int rainRaw = analogRead(rainPin);
  // Map 1024 (Dry) -> 0 (Wet) to 0% -> 100%
  int rainPercent = map(rainRaw, 1024, 0, 0, 100); 
  rainPercent = constrain(rainPercent, 0, 100); 

  Blynk.virtualWrite(V2, rainPercent); // V2 for Rainfall
  Serial.print("Rain: "); Serial.print(rainPercent); Serial.println("%");
}

void setup()
{
  Serial.begin(9600);
  Serial.println("\nStarting Weather Station...");

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  
  // Start Sensors
  dht.begin();
  
  // Initialize BMP280
  // Try generic address 0x76 first, then Adafruit address 0x77
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

  // Timer: Run the function every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop()
{
  Blynk.run();
  timer.run();
}
