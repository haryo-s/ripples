#!/usr/bin/env python

from samplebase import SampleBase
import urllib.request
import time

PANEL_WIDTH = 64
PANEL_HEIGHT = 32
URL = 'http://192.168.178.22:5000'

def bytes_to_boolean_array(data_bytes: bytes):
    #print(data_bytes)
    # Convert bytes into binary representation
    #binary_string = ''.join(format(byte, '08b') for byte in data_bytes)

    # Create a boolean array based on the binary representation
    #boolean_array = [bit == '1' for bit in binary_string]

    #return boolean_array
    return str(data_bytes, 'UTF-8')

class RipplesDisplay(SampleBase):
    def __init__(self, *args, **kwargs):
        super(RipplesDisplay, self).__init__(*args, **kwargs)

    def run(self):
        self.offscreen_canvas = self.matrix.CreateFrameCanvas()
        print(self.offscreen_canvas.width)
        print(self.offscreen_canvas.height)
        while True:
            with urllib.request.urlopen(URL) as response:
                boolean_array = bytes_to_boolean_array(response.read())
                self.offscreen_canvas.SetPixel(5, 5, 255, 255, 255)

                if len(boolean_array) == PANEL_WIDTH*PANEL_HEIGHT:
                    for idx, x in enumerate(boolean_array):
                        if x == "1":
                            self.offscreen_canvas.SetPixel(idx / self.offscreen_canvas.width, idx % self.offscreen_canvas.width, 128, 128, 128)
                        else:
                            self.offscreen_canvas.SetPixel(idx / self.offscreen_canvas.width, idx % self.offscreen_canvas.width, 0, 0, 0)
            self.offscreen_canvas = self.matrix.SwapOnVSync(self.offscreen_canvas)
    # int coord[2];
    # coord[0] = i % 96; //160 is referring to width
    # coord[1] = i / 96;

if __name__ == "__main__":
    ripples_display = RipplesDisplay()
    if (not ripples_display.process()):
        ripples_display.print_help()
