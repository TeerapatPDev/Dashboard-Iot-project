#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include "DFRobot_MICS.h"
#include <RF24.h>

Adafruit_BMP280 bmp;
Adafruit_AHTX0 myAHT20; // เปลี่ยนตรงนี้
DFRobot_MICS_ADC mics(A0, 2);

RF24 radio(9, 10);
const byte address[6] = "ALL01";

// โครงสร้างข้อมูลสำหรับการส่ง
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

SensorData data;

void setup() {
  Serial.begin(115200);
  Serial.println(F("AHT20+BMP280+MICS5524 with RF24 test"));

  // เริ่ม AHT20
  if (!myAHT20.begin()) {
    Serial.println(F("AHT20 not connected or failed to initialize!"));
    while (1) delay(10);
  }
  Serial.println(F("AHT20 OK"));

  // เริ่ม BMP280
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // เริ่ม MICS5524
  if (!mics.begin()) {
    Serial.println(F("MICS5524 not connected or fail to initialize!"));
    while (1);
  }
  Serial.println(F("MICS5524 connected successfully!"));

  if (mics.getPowerState() == SLEEP_MODE) {
    mics.wakeUpMode();
    Serial.println(F("MICS5524 woken up successfully!"));
  }
  while (!mics.warmUpTime(0.3)) {
    Serial.println(F("Waiting for sensor warm-up..."));
    delay(1000);
  }
  Serial.println(F("MICS5524 is ready!"));

  // เริ่ม RF24
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1);
  }
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
}

void loop() {
  // อ่าน AHT20
  sensors_event_t humidity, temp;
  myAHT20.getEvent(&humidity, &temp);

  data.temperature = temp.temperature;
  data.humidity = humidity.relative_humidity;

  // อ่าน BMP280
  data.pressure = bmp.readPressure();

  // อ่าน MICS5524
  data.coConcentration = mics.getGasData(CO);
  data.ch4Concentration = mics.getGasData(CH4);
  data.c2h5ohConcentration = mics.getGasData(C2H5OH);
  data.h2Concentration = mics.getGasData(H2);
  data.nh3Concentration = mics.getGasData(NH3);
  data.no2Concentration = mics.getGasData(NO2);

  // ส่งข้อมูล RF24
  if (radio.write(&data, sizeof(data))) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.println("Failed to send data.");
  }

  // แสดงข้อมูลใน Serial
  Serial.print("Temperature: "); Serial.print(data.temperature); Serial.println(" *C");
  Serial.print("Humidity: "); Serial.print(data.humidity); Serial.println(" %");
  Serial.print("Pressure: "); Serial.print(data.pressure); Serial.println(" Pa");

  Serial.print("CO: "); Serial.print(data.coConcentration); Serial.println(" ppm");
  Serial.print("CH4: "); Serial.print(data.ch4Concentration); Serial.println(" ppm");
  Serial.print("C2H5OH: "); Serial.print(data.c2h5ohConcentration); Serial.println(" ppm");
  Serial.print("H2: "); Serial.print(data.h2Concentration); Serial.println(" ppm");
  Serial.print("NH3: "); Serial.print(data.nh3Concentration); Serial.println(" ppm");
  Serial.print("NO2: "); Serial.print(data.no2Concentration); Serial.println(" ppm");

  Serial.println("-----------------------------");

  delay(1000);
}
