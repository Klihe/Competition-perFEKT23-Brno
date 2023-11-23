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
}

// Init variables
int oldEMGVal1 = 0;
int oldEMGVal2 = 0;

const float maxEMG1 = 200;
const float maxEMG2 = 200;

DCMotor motorA(MOT_A_DIR, MOT_A_SPEED);
DCMotor motorB(MOT_B_DIR, MOT_B_SPEED);

void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Set the motor directions
  motorA.setDir(FORWARD);
  motorB.setDir(BACKWARD);
}
 
void loop() {
  // Read sensor values
  int rawEMGVal1 = analogRead(EMG1) - 20;
  int rawEMGVal2 = analogRead(EMG2);

  int smoothedEMGVal1 = (rawEMGVal1 * 0.25) + (oldEMGVal1 * 0.75);
  int smoothedEMGVal2 = (rawEMGVal2 * 0.25) + (oldEMGVal2 * 0.75);

  // Save the values
  oldEMGVal1 = smoothedEMGVal1;
  oldEMGVal2 = smoothedEMGVal2;

  // Convert them to motor speeds
  int convertedVal1 = (1.0 * smoothedEMGVal1 / maxEMG1) * 255;
  int convertedVal2 = (1.0 * smoothedEMGVal2 / maxEMG2) * 255;

  if (smoothedEMGVal1 > 200) {
    convertedVal1 = 255;
  }
  if (smoothedEMGVal2 > 200) {
    convertedVal2 = 255;
  }

  // Log the values
  logValues("EMG_1_smooth:", smoothedEMGVal1, "EMG_2_smooth:", smoothedEMGVal2);
  logValues("EMG_1:", rawEMGVal1, "EMG_2:", rawEMGVal2);
  logValues("Reference_high:", 1000, "Reference_low:", 0);
  logValues("MotorA:", convertedVal1, "MotorB:", convertedVal2);
  Serial.println(" ");

  // Pass the speeds to the motors
  motorA.setSpeed(convertedVal1);
  motorB.setSpeed(convertedVal2);
}