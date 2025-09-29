from machine import Pin, I2C
import time
import pca9685

# I2C setup (Pico default I2C0 on GP0=SDA, GP1=SCL)
i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=400000)

# Init PCA9685
pwm = pca9685.PCA9685(i2c)
pwm.freq(1000)   # 1 kHz PWM frequency

# Motor connections
PWM_CH = 12          # PCA9685 OUT12 -> IN1
DIR_PIN = Pin(2, Pin.OUT)  # GP2 -> IN2

def motor_forward(duty=3000):
    DIR_PIN.value(0)             # Forward
    pwm.duty(PWM_CH, duty)       # Set PWM duty (0-4095)

def motor_reverse(duty=3000):
    DIR_PIN.value(1)             # Reverse
    pwm.duty(PWM_CH, duty)

def motor_stop():
    pwm.duty(PWM_CH, 0)          # Stop (0% duty)

# Main loop
while True:
    print("Forward 5s")
    motor_forward(3000)  # ~75% duty
    time.sleep(5)

    print("Stop 1s")
    motor_stop()
    time.sleep(1)

    print("Reverse 5s")
    motor_reverse(3000)
    time.sleep(5)

    print("Stop 1s")
    motor_stop()
    time.sleep(1)
