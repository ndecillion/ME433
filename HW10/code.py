import os
import board
import time
import digitalio
import analogio

from ulab import numpy as np

some_led = digitalio.DigitalInOut(board.GP16)
some_led.direction = digitalio.Direction.OUTPUT
some_button = digitalio.DigitalInOut(board.GP15) # put a button circuit on GP15
some_button.direction = digitalio.Direction.INPUT
built_in_led = digitalio.DigitalInOut(board.LED)
built_in_led.direction = digitalio.Direction.OUTPUT
adc_pin_a0 = analogio.AnalogIn(board.A0)

a = np.array([1,3,5,2,4,8,10])
# Test some stats functions, like
print(np.min(a))
print(np.max(a))
print(np.median(a))

combSin = np.empty([1024])
#combSin = []
for x in range(1024):
    worker = np.sin(x) + np.sin(2*x) + np.sin(3*x)
    combSin[x] = worker

real, imag = np.fft.fft(combSin)
for x in range((len(combSin)/2)): # To eliminate "mirrored" peaks in FFT plot
    time.sleep(0.02)
    print((real[x],))
#print(real)

#while 1:
#    print("("+str(adc_pin_a0.value)+",)") # print with plotting format
#    if (adc_pin_a0.value > 14000):
#        some_led.value = 1
#    else:
#        some_led.value = 0
#    if (some_button.value == 0):
#        built_in_led.value = 1
#    else:
#        built_in_led.value = 0
#    time.sleep(.05) # delay in seconds
#print("Hello World!")
