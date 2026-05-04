#https://www.raspberrypi.com/documentation/microcontrollers/micropython.html#where-can-i-find-documentation
#https://docs.micropython.org/en/latest/rp2/quickref.html#general-board-control
#https://docs.micropython.org/en/latest/library/rp2.html

#https://docs.micropython.org/en/latest/library/neopixel.html#class-neopixel
#https://raw.githubusercontent.com/micropython/micropython-lib/master/micropython/drivers/led/neopixel/neopixel.py
#https://github.com/micropython/micropython-lib

#https://github.com/adafruit/Adafruit_CircuitPython_NeoPixel/blob/main/neopixel.py
#https://github.com/blaz-r/pi_pico_neopixel

from machine import Pin
from neopixel import NeoPixel
import time

colors = {
  "green": [0, 150, 0],
  "red": [150, 0, 0],
  "blue":[0, 0, 150],
  "yellow": [150, 150, 0],
  "white": [255, 255, 255]
}

print()

#onpin = Pin(22, Pin.OUT)     # set GPIOX to output to drive NeoPixels
#onpin.value(0)

NeoRGB_PIN = 22   #RP2040 USB onBoard LED connected to GPIO pin 22

ledpin = Pin(NeoRGB_PIN)     # set GPIOX to output to drive NeoPixels
#ledpin.value(1)
#time.sleep(2)

print("NeoPixel.ctor")
np = NeoPixel(ledpin, 1, bpp=3, timing=(400, 850, 800, 450))  # create NeoPixel driver on GPIO0 for Y pixels
#ledpin.value(1)
#time.sleep(2)

print(f"NeoPixels {len(np)}")

#from apa106 import APA106
#ap = APA106(pin, 1)

print()
for name in colors.keys():
  time.sleep(1)
  print(f"{name} = {colors[name]}")
  #np[0] = (colors[name][0], colors[name][1], colors[name][2], 128)
  np.fill((colors[name][0], colors[name][1], colors[name][2], 128))
  np.write()              # write data to all pixels

time.sleep(1)
ledpin.value(1)
#onpin.value(1)
print("\nexit")
