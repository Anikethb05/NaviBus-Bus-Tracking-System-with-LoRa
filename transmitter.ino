#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

// The serial connection to the GPS module
SoftwareSerial gpsSerial(4, 5); // RX, TX
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println(F("GPS Module test"));

  // Initialize LoRa
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(20);
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Display GPS information
      displayInfo();

      // Send GPS data via LoRa
      sendData();
    }
  }

  // If 5000ms pass without any data, something may be wrong.
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print(F("Location: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
    Serial.print(F("Altitude: "));
    Serial.print(gps.altitude.meters());
    Serial.println(F("m"));
  } else {
    Serial.println(F("Location: NOT FOUND"));
  }

  if (gps.date.isValid()) {
    Serial.print(F("Date: "));
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.println(gps.date.year());
  }

  if (gps.time.isValid()) {
    Serial.print(F("Time: "));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    Serial.println(gps.time.second());
  }

  Serial.println();
}

void sendData() {
  // Send packet
  LoRa.beginPacket();
  LoRa.print("Latitude: ");
  LoRa.print(gps.location.lat(), 6);
  LoRa.print(", Longitude: ");
  LoRa.print(gps.location.lng(), 6);
  LoRa.print(", Altitude: ");
  LoRa.print(gps.altitude.meters());
  LoRa.println("m");
  LoRa.endPacket();

  delay(5000);
}