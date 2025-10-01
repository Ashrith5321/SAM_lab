#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Two PCA9685 boards
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // first PCA9685
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); // second PCA9685

// Define motor channel pairs for first 8 motors (pwm1)
const uint8_t motorIn1[8] = {0, 2, 4, 6, 8, 10, 12, 14};
const uint8_t motorIn2[8] = {1, 3, 5, 7, 9, 11, 13, 15};

// Motor 9 on second board (pwm2)
const uint8_t motor9_in1 = 0;
const uint8_t motor9_in2 = 1;

void setLow(Adafruit_PWMServoDriver &pwm, uint8_t ch) {
  pwm.setPWM(ch, 0, 0); // always low
}

void setHigh(Adafruit_PWMServoDriver &pwm, uint8_t ch) {
  pwm.setPWM(ch, 4096, 0); // always high
}

void setPWMd(Adafruit_PWMServoDriver &pwm, uint8_t ch, uint16_t duty) {
  if (duty == 0) {
    setLow(pwm, ch);
  } else if (duty >= 4095) {
    setHigh(pwm, ch);
  } else {
    pwm.setPWM(ch, 0, duty);
  }
}

void setMotor(int speed, Adafruit_PWMServoDriver &pwm, uint8_t in1_ch, uint8_t in2_ch) {
  if (speed > 0) {
    setPWMd(pwm, in1_ch, map(speed, 0, 255, 0, 4095));
    setLow(pwm, in2_ch);
  } else if (speed < 0) {
    setPWMd(pwm, in2_ch, map(-speed, 0, 255, 0, 4095));
    setLow(pwm, in1_ch);
  } else {
    setLow(pwm, in1_ch);
    setLow(pwm, in2_ch);
  }
}

void setup() {
  Wire.begin();
  pwm1.begin();
  pwm2.begin();
  pwm1.setPWMFreq(1000);
  pwm2.setPWMFreq(1000);

  // Stop all motors at start
  for (int i = 0; i < 8; i++) {
    setMotor(0, pwm1, motorIn1[i], motorIn2[i]);
  }
  setMotor(0, pwm2, motor9_in1, motor9_in2);
}

void loop() {
  // Forward all 9 motors
  for (int i = 0; i < 8; i++) {
    setMotor(200, pwm1, motorIn1[i], motorIn2[i]);
  }
  setMotor(200, pwm2, motor9_in1, motor9_in2);
  delay(2000);

  // Stop all 9 motors
  for (int i = 0; i < 8; i++) {
    setMotor(0, pwm1, motorIn1[i], motorIn2[i]);
  }
  setMotor(0, pwm2, motor9_in1, motor9_in2);
  delay(1000);

  // Reverse all 9 motors
  for (int i = 0; i < 8; i++) {
    setMotor(-200, pwm1, motorIn1[i], motorIn2[i]);
  }
  setMotor(-200, pwm2, motor9_in1, motor9_in2);
  delay(2000);

  // Stop all 9 motors
  for (int i = 0; i < 8; i++) {
    setMotor(0, pwm1, motorIn1[i], motorIn2[i]);
  }
  setMotor(0, pwm2, motor9_in1, motor9_in2);
  delay(2000);
}
