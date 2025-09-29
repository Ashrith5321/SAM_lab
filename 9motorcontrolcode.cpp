#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Two PCA9685 boards: addresses must match your jumpers (A0..A5).
Adafruit_PWMServoDriver pwmA = Adafruit_PWMServoDriver(0x40); // 16 ch used
Adafruit_PWMServoDriver pwmB = Adafruit_PWMServoDriver(0x41); // 2 ch used

// Map each motor to (driver, in1Channel, in2Channel).
// Motors 0..7 on pwmA using pairs (0,1), (2,3), ... (14,15)
// Motor 8 on pwmB using (0,1)
struct ChanMap { Adafruit_PWMServoDriver* drv; uint8_t in1; uint8_t in2; };

ChanMap motorMap[9] = {
  { &pwmA,  0,  1 }, // M0
  { &pwmA,  2,  3 }, // M1
  { &pwmA,  4,  5 }, // M2
  { &pwmA,  6,  7 }, // M3
  { &pwmA,  8,  9 }, // M4
  { &pwmA, 10, 11 }, // M5
  { &pwmA, 12, 13 }, // M6
  { &pwmA, 14, 15 }, // M7
  { &pwmB,  0,  1 }  // M8
};

// Helper: set a PCA9685 channel to a duty (0..4095).
// Uses setPWM(on=0, off=duty). 0 => always LOW, 4095 => ~always HIGH.
inline void setDuty(Adafruit_PWMServoDriver* drv, uint8_t ch, uint16_t duty) {
  if (duty >= 4095) {
    // Full ON (HIGH) — use special mode to avoid jitter
    drv->setPWM(ch, 4096, 0);
  } else if (duty == 0) {
    // Full OFF (LOW)
    drv->setPWM(ch, 0, 0);
  } else {
    drv->setPWM(ch, 0, duty);
  }
}

// Set one motor speed and direction.
// motorIndex: 0..8
// speedPercent: -100..+100 (0 = coast). magnitude controls speed.
// If you prefer "brake" at zero, flip COAST_AT_ZERO to false.
const bool COAST_AT_ZERO = true;

void setMotor(int motorIndex, int speedPercent) {
  if (motorIndex < 0 || motorIndex >= 9) return;
  ChanMap m = motorMap[motorIndex];

  // Clamp
  if (speedPercent > 100) speedPercent = 100;
  if (speedPercent < -100) speedPercent = -100;

  // Map 0..100% to 0..4095 duty
  uint16_t duty = (uint16_t)((abs(speedPercent) * 4095L) / 100);

  if (speedPercent > 0) {
    // Forward: IN1 = PWM, IN2 = LOW
    setDuty(m.drv, m.in1, duty);
    setDuty(m.drv, m.in2, 0);
  } else if (speedPercent < 0) {
    // Reverse: IN1 = LOW, IN2 = PWM
    setDuty(m.drv, m.in1, 0);
    setDuty(m.drv, m.in2, duty);
  } else {
    if (COAST_AT_ZERO) {
      // Coast: both LOW
      setDuty(m.drv, m.in1, 0);
      setDuty(m.drv, m.in2, 0);
    } else {
      // Brake: both HIGH
      setDuty(m.drv, m.in1, 4095);
      setDuty(m.drv, m.in2, 4095);
    }
  }
}

void stopAll() {
  for (int i = 0; i < 9; ++i) setMotor(i, 0);
}

void setup() {
  Wire.begin(5,6);            // Use default Pico I2C pins (SDA/SCL you wired)
  Wire.setClock(400000);   // Fast-mode I2C (optional; PCA9685 supports it)

  pwmA.begin();
  pwmB.begin();

  // Set PWM frequency for motors.
  // PCA9685 max ~1526 Hz. Use a high value to reduce audible whine.
  pwmA.setPWMFreq(1500);   // ~1.5 kHz
  pwmB.setPWMFreq(1500);

  // Ensure all off at start
  stopAll();

  // Example: spin all forward at 30% for 2s, then reverse 30% for 2s, then stop
  for (int i = 0; i < 9; ++i) setMotor(i, 30);
  delay(2000);
  for (int i = 0; i < 9; ++i) setMotor(i, -30);
  delay(2000);
  stopAll();
}

void loop() {
  // Demo: ramp motor 0 from 0 → 100% → 0, forward; then reverse
  for (int s = 0; s <= 100; s += 5) { setMotor(0, s); delay(50); }
  for (int s = 100; s >= 0; s -= 5) { setMotor(0, s); delay(50); }
  for (int s = 0; s >= -100; s -= 5) { setMotor(0, s); delay(50); }
  for (int s = -100; s <= 0; s += 5) { setMotor(0, s); delay(50); }

  delay(500);
}
