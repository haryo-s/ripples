#!/usr/bin/env python

from samplebase import SampleBase
import urllib.request
import time

# PANEL_WIDTH = 64
# PANEL_HEIGHT = 32
# URL = 'http://192.168.178.22:5000'

class LEDPanelTest(SampleBase):
    def __init__(self, *args, **kwargs):
        super(LEDPanelTest, self).__init__(*args, **kwargs)

    def run(self):
        self.offscreen_canvas = self.matrix.CreateFrameCanvas()

        while True:
            self.usleep(5 * 1000)
            self.offscreen_canvas.Fill(255, 0, 0)
            self.offscreen_canvas = self.matrix.SwapOnVSync(self.offscreen_canvas)

if __name__ == "__main__":
    led_panel_test = LEDPanelTest()
    if (not led_panel_test.process()):
        led_panel_test.print_help()
