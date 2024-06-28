#include <Wire.h>
#include <MPU6050_light.h>
#include <Servo.h>

MPU6050 mpu(Wire);

Servo esc;  // ESC for BLDC motor
Servo servoFlywheel;  // Servo for controlling flywheel orientation
Servo servoStructure;  // Servo for controlling overall structure orientation

int escPin = 11;        // ESC signal pin
int servoFlywheelPin = 10; // Servo signal pin for flywheel
int servoStructurePin = 6;  // Servo signal pin for overall structure

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.begin();
  delay(2000);  // Allow MPU6050 to stabilize
  mpu.calcGyroOffsets();  // Calculate gyro offsets for accurate readings
  
  // Initialize devices
  esc.attach(escPin);            // Attach ESC for BLDC motor
  servoFlywheel.attach(servoFlywheelPin);    // Attach servo for flywheel orientation
  servoStructure.attach(servoStructurePin);  // Attach servo for overall structure orientation

  // Arm and initialize ESC for BLDC motor
  esc.writeMicroseconds(850);  // Minimum signal to arm ESC
  delay(2000);                   // Wait for ESC initialization
  esc.writeMicroseconds(1600);  // Set desired constant speed for BLDC motor
  
  // Initialize servos to neutral positions
  servoFlywheel.write(90);      // Neutral position for flywheel orientation
  servoStructure.write(90);     // Neutral position for structure orientation
}

void loop() {
  mpu.update();  // Update MPU6050 readings
  
  // Example control logic for flywheel orientation based on MPU6050 pitch
  float pitch = mpu.getAngleX();  // Get X angle (pitch)
  int servoFlywheelPos = map(pitch, -90, 90, 0, 180);
  servoFlywheel.write(servoFlywheelPos);
  
  // Additional control logic for overall structure orientation
  // This example uses roll, but you may adjust based on your control needs
  float roll = mpu.getAngleY();  // Get Y angle (roll)
  int servoStructurePos = map(roll, -90, 90, 0, 180);
  servoStructure.write(servoStructurePos);

  // Debugging Outputs
  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print("\tFlywheel Servo Position: "); Serial.print(servoFlywheelPos);
  Serial.print("\tRoll: "); Serial.print(roll);
  Serial.print("\tStructure Servo Position: "); Serial.println(servoStructurePos);

  delay(100);  // Adjust loop delay as needed