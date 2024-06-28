#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LoRa.h>

// Pins for GPS
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
#define SEALEVELPRESSURE_HPA (1013.25)

// Create objects for the sensors
Adafruit_MPU6050 mpu;
Adafruit_BME680 bme;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
int counter=0;
void setup() {
  
  Serial.begin(115200);
  ss.begin(GPSBaud);
  LoRa.begin(433E6);

  // Initialize MPU6050
  mpu.begin();
  //Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize BME680
  bme.begin();
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms

  Serial.println(F(""));
}

void loop() {
  // Read GPS data
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Read MPU6050 data
  sensors_event_t a, g, temp_mpu;
  mpu.getEvent(&a, &g, &temp_mpu);

  // Read BME680 data
  bme.performReading();

  // Print GPS data
  //if (gps.location.isValid()) {
    //Serial.print("Latitude: ");
    //Serial.println(gps.location.lat(), 6);
    float lati=gps.location.lat();
    //Serial.print("Longitude: ");
    float longi=gps.location.lng();
    //Serial.println(gps.location.lng(), 6);
  //} else {
    //Serial.println("Location not valid");
    //Serial.println("");
  //}

  //if (gps.altitude.isValid()) {
    //Serial.print("Altitude: ");
    //Serial.print(gps.altitude.meters());
    float alt=gps.altitude.meters();
    //Serial.println(" meters");
  //} else {
    //Serial.println("Altitude not valid");
    //Serial.println("");
  //}

  // Additional GPS Debugging Info
  /*Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("HDOP: ");
  Serial.println(gps.hdop.value());*/

  // Print MPU6050 data
  //Serial.print("Acceleration X: ");
  float ax= a.acceleration.x;
  float ay=a.acceleration.y;
  float az= a.acceleration.z;
  //Serial.print(a.acceleration.x);
  //Serial.print(", Y: ");
  //Serial.print(a.acceleration.y);
  //Serial.print(", Z: ");
  //Serial.print(a.acceleration.z);
  //Serial.println(" m/s^2");

  /*Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");*/
  float gx=g.gyro.x;
  float gy=g.gyro.y;
  float gz=g.gyro.z;

  /*Serial.print("Temperature (MPU6050): ");
  Serial.print(temp_mpu.temperature);
  Serial.println(" degC");*/
  float tem=temp_mpu.temperature;

  // Print BME680 data
  /*Serial.print("Temperature (BME680): ");
  Serial.print(bme.temperature);
  Serial.println(" *C");*/
  float btem=bme.temperature;

  /*Serial.print("Pressure: ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");*/
  float press=bme.pressure;

  /*Serial.print("Humidity: ");
  Serial.print(bme.humidity);
  Serial.println(" %");*/
  float hum=bme.humidity;

  /*Serial.print("Gas: ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");*/

  /*Serial.print("Approx. Altitude: ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");*/
  float balt=bme.readAltitude(SEALEVELPRESSURE_HPA);
  String data = String(lati)+","+String(longi)+","+String(alt)+","+String(ax)+","+String(ay)+","+String(az)+","+String(gx)+","+String(gy)+","+String(gz)+","+String(tem)+","+String(btem)+","+String(press)+","+String(hum)+","+String(balt);

  Serial.println(data);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  counter++;
  delay(500);
}
