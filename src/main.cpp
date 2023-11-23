#include <Arduino.h>
#include <Config.h>

// Class for easily controlling the dc motors
class DCMotor {
  public:
    int _dirPin;
    int _speedPin;

    DCMotor(int dirPin, int speedPin) {
      _dirPin = dirPin;
      _speedPin = speedPin;
    }

    void setSpeed(int newSpeed) {
      analogWrite(_speedPin, newSpeed);
    }

    void setDir(int newDir) {
      digitalWrite(_dirPin, newDir);
    }
};

// Function for logging EMG values into graph
void logValues(String label1, int emgVal1, String label2, int emgVal2) {
  Serial.print(label1);
  Serial.print(emgVal1);
  Serial.print(",");

  Serial.print(label2);
  Serial.print(emgVal2);
  Serial.print(",");

  Serial.print("Reference high:");
  Serial.print(1000);
  Serial.print(",");
  Serial.print("Reference low:");
  Serial.println(0);
}

// Init variables
int oldEMGVal1 = 0;
int oldEMGVal2 = 0;

const int maxEMG1 = 600;
const int maxEMG2 = 600;

DCMotor motorA(MOT_A_DIR, MOT_A_SPEED);
DCMotor motorB(MOT_B_DIR, MOT_B_SPEED);

void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Set the motor directions
  motorA.setDir(FORWARD);
  motorB.setDir(FORWARD);
}
 
void loop() {
  // Read sensor values
  int rawEMGVal1 = analogRead(EMG1);
  int rawEMGVal2 = analogRead(EMG2);

  int smoothedEMGVal1 = (rawEMGVal1 * 0.25) + (oldEMGVal1 * 0.75);
  int smoothedEMGVal2 = (rawEMGVal2 * 0.25) + (oldEMGVal2 * 0.75);

  // Save the values
  oldEMGVal1 = smoothedEMGVal1;
  oldEMGVal2 = smoothedEMGVal2;

  // Log the values
  logValues("EMG_1_smooth:", smoothedEMGVal1, "EMG_2_smooth:", smoothedEMGVal2);
  logValues("EMG_1:", rawEMGVal1, "EMG_2:", rawEMGVal2);

  // Convert them to motor speeds
  int convertedVal1 = (smoothedEMGVal1 / maxEMG1) * 255;
  int convertedVal2 = (smoothedEMGVal2 / maxEMG2) * 255;

  // Pass the speeds to the motors
  motorA.setSpeed(convertedVal1);
  motorB.setSpeed(convertedVal2);
}