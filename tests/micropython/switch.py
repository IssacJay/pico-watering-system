"""
MicroPython switch test for Raspberry Pi Pico

- Pins: GPIO 14 = BURST, GPIO 15 = RESET (active-low with internal pull-ups)
- LED: GPIO 25 (onboard)

Usage:
- Copy this file to the Pico and run it (Thonny, mpremote, or paste to REPL).
- Observe serial output and LED when a switch is detected.
- To simulate a press without a switch, connect GPIO 14 (or 15) directly to GND.

This script prints raw samples and debounced state, and detects long press on
RESET (hold) to mimic the C behaviour.
"""

from machine import Pin
import time

# Configuration
PIN_BURST = 14
PIN_RESET = 15
LED_PIN = 25

# Debounce settings
DEBOUNCE_SAMPLES = 3
DEBOUNCE_DELAY_MS = 10
LONG_PRESS_MS = 1500

# Active-low: 0 when pressed
ACTIVE_LEVEL = 0
INACTIVE_LEVEL = 1

burst_pin = Pin(PIN_BURST, Pin.IN, Pin.PULL_UP)
reset_pin = Pin(PIN_RESET, Pin.IN, Pin.PULL_UP)
led = Pin(LED_PIN, Pin.OUT)


def read_debounced(pin, samples=DEBOUNCE_SAMPLES, delay_ms=DEBOUNCE_DELAY_MS):
    """Sample the pin `samples` times separated by `delay_ms` and
    return the stable value if all samples match, otherwise return None."""
    first = pin.value()
    for _ in range(samples - 1):
        time.sleep_ms(delay_ms)
        if pin.value() != first:
            return None
    return first


def is_active(pin):
    """Return True if the pin is considered active (pressed) using debounce."""
    v = read_debounced(pin)
    if v is None:
        return None
    return v == ACTIVE_LEVEL


print("Starting MicroPython switch test. Press Ctrl-C to stop.")
print("BURST -> GPIO %d, RESET -> GPIO %d, LED -> GPIO %d" % (PIN_BURST, PIN_RESET, LED_PIN))
print("Note: Pins use internal pull-ups; connect pin to GND to simulate press.")

try:
    last_burst_state = INACTIVE_LEVEL
    last_reset_state = INACTIVE_LEVEL
    reset_press_start = None

    while True:
        # Raw quick sample for debugging
        raw_burst = burst_pin.value()
        raw_reset = reset_pin.value()

        deb_burst = read_debounced(burst_pin)
        deb_reset = read_debounced(reset_pin)

        # Convert debounced reading to readable state
        if deb_burst is None:
            burst_state_str = 'bounce'
            burst_active = None
        else:
            burst_active = (deb_burst == ACTIVE_LEVEL)
            burst_state_str = 'ACTIVE' if burst_active else 'INACTIVE'

        if deb_reset is None:
            reset_state_str = 'bounce'
            reset_active = None
        else:
            reset_active = (deb_reset == ACTIVE_LEVEL)
            reset_state_str = 'ACTIVE' if reset_active else 'INACTIVE'

        print('RAW: burst=%d reset=%d | DEB: burst=%s reset=%s' % (raw_burst, raw_reset, burst_state_str, reset_state_str))

        # Burst action: momentary
        if burst_active is True and last_burst_state == INACTIVE_LEVEL:
            print('BURST: detected -> starting burst action')
            led.toggle()
            time.sleep_ms(200)
            led.toggle()

        # Reset action: require long press
        if reset_active is True:
            if reset_press_start is None:
                reset_press_start = time.ticks_ms()
            else:
                held_ms = time.ticks_diff(time.ticks_ms(), reset_press_start)
                if held_ms >= LONG_PRESS_MS:
                    print('RESET: long-press detected (%d ms) -> resetting elapsed time' % held_ms)
                    # visual feedback
                    for _ in range(3):
                        led.on()
                        time.sleep_ms(150)
                        led.off()
                        time.sleep_ms(150)
                    reset_press_start = None
        else:
            reset_press_start = None

        # Update last stable states
        if burst_active is not None:
            last_burst_state = INACTIVE_LEVEL if not burst_active else ACTIVE_LEVEL
        if reset_active is not None:
            last_reset_state = INACTIVE_LEVEL if not reset_active else ACTIVE_LEVEL

        time.sleep_ms(150)

except KeyboardInterrupt:
    print('\nStopping switch test.')
    led.off()

