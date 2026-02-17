from machine import Pin, PWM
import time

RPWM_PIN = 26  # GPIO pin for extending
LPWM_PIN = 27  # GPIO pin for retracting

def test_led(pin_number=2, delay_on=2, delay_off=1, duration=10):
    """
    Function to test the LED for a specified duration.
    Args:
        pin_number: GPIO pin number for the LED (default 2)
        delay_on: Time in seconds the LED stays on (default 2)
        delay_off: Time in seconds the LED stays off (default 1)
        duration: Total time to run the test in seconds (default 10)
    """
    # Initialize LED
    led = Pin(pin_number, Pin.OUT)
    
    print(f"Starting LED test on GPIO {pin_number} for {duration} seconds")
    
    start_time = time.time()
    try:
        # Blink loop
        while time.time() - start_time < duration:
            led.value(1)
            time.sleep(delay_on)
            if time.time() - start_time >= duration:
                break
            led.value(0)
            time.sleep(delay_off)
    except KeyboardInterrupt:
        print("Test stopped")
    finally:
        led.value(0) # Ensure LED is off

def test_pwm(pin_number=2, duration=10):
    """
    Function to test PWM for a specified duration.
    Args:
        pin_number: GPIO pin number for PWM (default 26)
        duration: Total time to run the test in seconds (default 10)
    """    
    # Setup PWM on GPIO 26 (Connect your LED or Driver pin here)
    # freq=1000 means the pulse happens 1000 times a second (standard for motors)
    actuator_pwm = PWM(Pin(pin_number), freq=1000)
    
    print(f"Starting PWM Test for {duration} seconds...")
    
    start_time = time.time()
    try:
        while time.time() - start_time < duration:
            # 1. Ramp Up (Slow to Fast)
            print("Speeding up...")
            for speed in range(0, 65535, 1000): # Step by 1000
                if time.time() - start_time >= duration:
                    break
                actuator_pwm.duty_u16(speed)
                time.sleep(0.05) # Wait 50ms to see the change
            
            if time.time() - start_time >= duration:
                break

            # 2. Hold at Full Speed
            print("Full Speed!")
            actuator_pwm.duty_u16(65535)
            time.sleep(1)

            if time.time() - start_time >= duration:
                break

            # 3. Ramp Down (Fast to Slow)
            print("Slowing down...")
            for speed in range(65535, 0, -1000):
                if time.time() - start_time >= duration:
                    break
                actuator_pwm.duty_u16(speed)
                time.sleep(0.05)
                
            # 4. Stop
            actuator_pwm.duty_u16(0)
            time.sleep(1)
    except KeyboardInterrupt:
        print("Test stopped")
    finally:
        actuator_pwm.duty_u16(0) # Ensure PWM is off

def run_one_cycle(r_pwm_pin=26, l_pwm_pin=27, speed=100, duration=2):
    """
    Performs exactly one cycle: Extend -> Pause -> Retract -> Stop.
    
    Args:
        r_pwm_pin (int): Pin for extending (RPWM)
        l_pwm_pin (int): Pin for retracting (LPWM)
        speed (int): Speed percentage (0-100)
        duration (float): Time in seconds to move in each direction
    """
    print(f"--- STARTING SINGLE CYCLE (GPIO {r_pwm_pin}/{l_pwm_pin}) ---")
    
    # Initialize PWM on the pins
    extend_pwm = PWM(Pin(r_pwm_pin), freq=1000)
    retract_pwm = PWM(Pin(l_pwm_pin), freq=1000)
    
    # Convert 0-100 speed to 0-65535 duty cycle
    duty_cycle = int(speed * 65535 / 100)
    
    try:
        # 1. EXTEND
        print(f"Extending for {duration} seconds...")
        retract_pwm.duty_u16(0)       # Ensure other side is off
        extend_pwm.duty_u16(duty_cycle)
        time.sleep(duration)
        
        # 2. PAUSE (Soft Stop)
        print("Pausing...")
        extend_pwm.duty_u16(0)
        time.sleep(0.5)               # Brief pause to protect gears
        
        # 3. RETRACT
        print(f"Retracting for {duration} seconds...")
        retract_pwm.duty_u16(duty_cycle)
        time.sleep(duration)
        
    except KeyboardInterrupt:
        print("\n! CYCLE INTERRUPTED BY USER !")
        
    finally:
        # 4. HARD STOP (Runs no matter what)
        print("--- CYCLE COMPLETE: MOTORS OFF ---")
        extend_pwm.duty_u16(0)
        retract_pwm.duty_u16(0)
        
        # Optional: De-initialize to release pins (good practice for one-off tasks)
        extend_pwm.deinit()
        retract_pwm.deinit()

def extend_only(duration=2, speed=100):
    """
    Extends the actuator for a specific time, then stops.
    """
    print(f"--> Extending for {duration}s at {speed}%...")
    
    # 1. Setup
    ext = PWM(Pin(RPWM_PIN), freq=1000)
    # ret = PWM(Pin(LPWM_PIN), freq=1000)
    duty = int(speed * 65535 / 100)
    
    try:
        # 2. Safety: Ensure Retract is 0 before enabling Extend
        # ret.duty_u16(0)
        ext.duty_u16(duty)
        
        # 3. Run
        time.sleep(duration)
        
    except KeyboardInterrupt:
        print("Interrupted!")
        
    finally:
        # 4. Stop & Clean up
        ext.duty_u16(0)
        # ret.duty_u16(0)
        ext.deinit()
        # ret.deinit()
        print("--> Stopped.")

def retract_only(duration=2, speed=100):
    """
    Retracts the actuator for a specific time, then stops.
    """
    print(f"<-- Retracting for {duration}s at {speed}%...")
    
    # 1. Setup
    ext = PWM(Pin(RPWM_PIN), freq=1000)
    ret = PWM(Pin(LPWM_PIN), freq=1000)
    duty = int(speed * 65535 / 100)
    
    try:
        # 2. Safety: Ensure Extend is 0 before enabling Retract
        ext.duty_u16(0)
        ret.duty_u16(duty)
        
        # 3. Run
        time.sleep(duration)
        
    except KeyboardInterrupt:
        print("Interrupted!")
        
    finally:
        # 4. Stop & Clean up
        ext.duty_u16(0)
        ret.duty_u16(0)
        ext.deinit()
        ret.deinit()
        print("<-- Stopped.")

if __name__ == "__main__":
    # test_led()
    # test_pwm()  # Uncomment to run PWM test
    # extend_only(duration=2, speed=50)  # Example: Extend for 2 seconds at 50% speed
    # retract_only(duration=2, speed=50)  # Example: Retract for 2 seconds at 50% speed