from machine import Pin, PWM
import utime

# config

CPR = 0
Quad = 0
Gear = 0
V_supply = 0
PWM_FREQ = 0
Window = 0
Duty_start = 0
Duty_stop = 0
Duty_step = 0

#pins

PIN_PWM = 2
PIN_DIR1 = 3
PIN_DIR2 = 4
PIN_ENCODER_A = 14
PIN_ENCODER_B = 15

#motor control
pwm = PWM(Pin(PIN_PWM))
pwm.freq(PWM_FREQ)

direction1 = Pin(PIN_DIR1, Pin.OUT)
direction2 = Pin(PIN_DIR2, Pin.OUT)

def set_direction(forward: bool = True):
    if forward:
        direction1.value(1)
        direction2.value(0)
    else:
        direction1.value(0)
        direction2.value(1)
        
def set_duty_percentage(percentage: int):
    percentage = max(0, min(100, percentage))
    pwm.duty_u16(int(percentage * 65535 // 100))# i dont know wht i did her


encoder_a = Pin(PIN_ENCODER_A, Pin.IN, Pin.PULL_UP)
encoder_b = Pin(PIN_ENCODER_B, Pin.IN, Pin.PULL_UP)

count = 0

def isr_a(pin):
    global count 
    a = encoder_a.value()
    b = encoder_b.value()
    
    if a == b:
        count += 1
    else:
        count -= 1
        
        
def isr_b(pin):
    global count
    a = encoder_a.value()
    b = encoder_b.value()
    
    if a != b:
        count += 1
    else:
        count -= 1
        
encoder_a.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FAILING, handler = isr_a)
encoder_b.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FAILING, handler = isr_b)

EDGES_PER_REVOLUTION = CPR * Quad * Gear

def main():
    print("start")
    set_direction(True)
    set_duty_percentage(0.5)
    
    for duty in range(Duty_start, Duty_stop+1, Duty_step):
        global count 
        start = count
        
        set_duty_percentage(duty)
        utime.sleep(Window)
        
        delta = count - start
        rps = (delta / EDGES_PER_REVOLUTION) / Window if EDGES_PER_REVOLUTION> 0 else 0.0
        rpm = 60 * rps
        veff = V_supply * (duty/100)
        
        print(f"{duty}, {veff:.3f},{rpm:,2f}")
    
#stop motor
    
if __name__ == "__main__":
    main()
