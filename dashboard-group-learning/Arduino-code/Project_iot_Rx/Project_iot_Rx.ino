#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "ALL01";

// โครงสร้างข้อมูลสำหรับการรับ
struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  float coConcentration;
  float ch4Concentration;
  float c2h5ohConcentration;
  float h2Concentration;
  float nh3Concentration;
  float no2Concentration;
};

SensorData receivedData;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Receiver Ready"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1);
  }
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();

  Serial.println(F("Receiver is listening for data..."));
}

void loop() {
  // ตรวจสอบว่ามีข้อมูลเข้ามาหรือไม่
  if (radio.available()) {
    // อ่านข้อมูลที่ส่งมา
    radio.read(&receivedData, sizeof(receivedData));

    // ส่งข้อมูลผ่าน Serial
    Serial.print(receivedData.temperature, 2); Serial.print(",");
    Serial.print(receivedData.humidity, 2); Serial.print(",");
    Serial.print(receivedData.pressure, 2); Serial.print(",");
    Serial.print(receivedData.coConcentration, 2); Serial.print(",");
    Serial.print(receivedData.ch4Concentration, 2); Serial.print(",");
    Serial.print(receivedData.c2h5ohConcentration, 2); Serial.print(",");
    Serial.print(receivedData.h2Concentration, 2); Serial.print(",");
    Serial.print(receivedData.nh3Concentration, 2); Serial.print(",");
    Serial.print(receivedData.no2Concentration, 2);
    Serial.println();
  } else {
    Serial.println(F("No data received"));
  }
  delay(1000);
}
