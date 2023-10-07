#!/bin/bash 

sudo python pi/pi_receiver.py --led-rows 32 --led-cols 64 --led-no-hardware-pulse 1 --led-slowdown-gpio 3 --led-gpio-mapping=adafruit-hat --remote-url "http://pi4b:5000/"