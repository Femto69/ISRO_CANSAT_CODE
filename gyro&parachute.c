#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <MPU6050_light.h>
#include <Servo.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Sensors
Adafruit_BME680 bme; // I2C for BME680
MPU6050 mpu(Wire); // I2C for MPU6050

// Servos
Servo esc; // ESC for BLDC motor
Servo servoFlywheel; // Servo for controlling flywheel orientation
Servo servoStructure; // Servo for controlling overall structure orientation
Servo myservo;
// Pins
int escPin = 11;
int servoFlywheelPin = 10;
int servoStructurePin = 6;
int myservopin=12;

// BME680 altitude tracking
bool hasRotated = false;
float initialAltitude = -1;
float descentDepth = 2.0; // Change in altitude to trigger servo

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize BME680
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // BME680 sensor settings
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  // Initialize MPU6050
  mpu.begin();
  delay(2000);
  mpu.calcGyroOffsets();

  // Attach servos
  esc.attach(escPin);
  servoFlywheel.attach(servoFlywheelPin);
  servoStructure.attach(servoStructurePin);
  myservo.attach(myservopin);

  // Initialize ESC for BLDC motor
  esc.writeMicroseconds(850); // Minimum signal to arm ESC
  delay(2000);
  esc.writeMicroseconds(1600); // Set constant speed

  // First reading to set initial altitude
  if (bme.performReading()) {
    initialAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  }
}

void loop() {
  // Update sensors
  bme.performReading();
  mpu.update();

  // Handle altitude and servo for descent
  float currentAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (!hasRotated && (initialAltitude - currentAltitude) >= descentDepth) {
    myservo.write(90); // Rotate to some degree
    delay(1000);
    myservo.write(0); // Reset position
    hasRotated = true; // Prevent further rotation
  }

  // Orientation control based on MPU6050
  float pitch = mpu.getAngleX(); // Control flywheel orientation
  float roll = mpu.getAngleY(); // Control structure orientation
  servoFlywheel.write(map(pitch, -90, 90, 0, 180));
  servoStructure.write(map(roll, -90, 90, 0, 180));

  // Debugging outputs
  Serial.print("Altitude: "); Serial.print(currentAltitude);
  Serial.print(", Pitch: "); Serial.print(pitch);
  Serial.print(", Roll: "); Serial.println(roll);

  delay(100); // Manage loop timing
}