# MicroPython USB Keyboard example
#
# To run this example:
#
# 1. Check the KEYS assignment below, and connect buttons or switches to the
#    assigned GPIOs. You can change the entries as needed, look up the reference
#    for your board to see what pins are available. Note that the example uses
#    "active low" logic, so pressing a switch or button should switch the
#    connected pin to Ground (0V).
#
# 2. Make sure `usb-device-keyboard` is installed via: mpremote mip install usb-device-keyboard
#
# 3. Run the example via: mpremote run keyboard_example.py
#
# 4. mpremote will exit with an error after the previous step, because when the
#    example runs the existing USB device disconnects and then re-enumerates with
#   the keyboard interface present. At this point, the example is running.
#
# 5. The example doesn't print anything to the serial port, but to stop it first
#    re-connect: mpremote connect PORTNAME
#
# 6. Type Ctrl-C to interrupt the running example and stop it. You may have to
#    also type Ctrl-B to restore the interactive REPL.
#
# To implement a keyboard with different USB HID characteristics, copy the
# usb-device-keyboard/usb/device/keyboard.py file into your own project and modify
# KeyboardInterface.
#
# MIT license; Copyright (c) 2024 Angus Gratton
import usb.device
from usb.device.keyboard import KeyboardInterface, KeyCode
import time
from machine import Pin

ONBOARD_LED_PIN = 22


class AutoKeyboard():
    def __init__(self, led_pin_nr):
        self.led_pin = Pin(led_pin_nr, Pin.OUT)
        self.led_pin.value(1)  #led off means to set output to high

    def led_flash(self):
        self.led_pin.value(0)  #led on means to set output to low
        time.sleep_ms(10)
        self.led_pin.value(1)  #led off means to set output to high

    def led_flash_3times_and_wait(self):
        self.led_flash()
        time.sleep_ms(500)
        self.led_flash()
        time.sleep_ms(500)
        self.led_flash()
        time.sleep_ms(5000)

    def run(self):
        print("")
        print("Registering keyboard class...")
      
        # Register the keyboard interface and re-enumerate
        k = KeyboardInterface()
        dev = usb.device.get()
        dev.init(k, builtin_driver=True)
    
        while not k.is_open():
            print("Opening keyboard...")
            #self.led_flash()
            time.sleep_ms(500)
    
        print("Entering keyboard loop...")
        self.led_flash_3times_and_wait()

        keys = [KeyCode.N0, KeyCode.N8, KeyCode.N1, KeyCode.N5, KeyCode.N0, KeyCode.N8, KeyCode.N1, KeyCode.N5, KeyCode.ENTER, KeyCode.ESCAPE]
      
        if k.is_open():
            for i in range(6):
                time.sleep_ms(2000)
                #k.send_keys(keys)
                for key in keys:
                    k.send_keys([key])
                    self.led_flash()

        dev.active(False)
        self.led_flash_3times_and_wait()


app = AutoKeyboard(ONBOARD_LED_PIN)
app.run()
