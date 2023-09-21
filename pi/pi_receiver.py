#!/usr/bin/env python

from samplebase import SampleBase
from random import randrange
import urllib.request
import time

class RipplesDisplay(SampleBase):
    def __init__(self, *args, **kwargs):
        super(RipplesDisplay, self).__init__(*args, **kwargs)
        self.PANEL_WIDTH  = 64
        self.PANEL_HEIGHT = 32
        self.PANEL_LENGTH = self.PANEL_WIDTH * self.PANEL_HEIGHT
        self.URL_LOCAL    = 'http://127.0.0.1:5000'
        self.URL_REMOTE   = 'http://192.168.178.22:5000'
        self.USE_LOCAL    = True
        self.USE_REMOTE   = True

    def run(self):
        self.offscreen_canvas = self.matrix.CreateFrameCanvas()
        self.width = self.offscreen_canvas.width
        self.height = self.offscreen_canvas.height

        self.local_difference_image = ""
        self.remote_difference_image = ""
        
        while True:
            if self.USE_LOCAL:
                self.local_difference_image = self.get_camera_difference_image(self.URL_LOCAL)
                print("Getting local image")
                if len(self.local_difference_image) != self.PANEL_LENGTH:
                    break
            if self.USE_REMOTE:
                print("Getting remote image")
                self.remote_difference_image = self.get_camera_difference_image(self.URL_REMOTE)
                if len(self.remote_difference_image) != self.PANEL_LENGTH:
                    break

            # Only use local and display its difference image
            if self.USE_LOCAL == True and self.USE_REMOTE == False:
                self.display_difference_image(self.local_difference_image)
                print("Using local only")
            # Only use remote and display its difference image
            if self.USE_LOCAL == False and self.USE_REMOTE == True:
                self.display_difference_image(self.remote_difference_image)
                print("Using remote only")

            # Ok, a bit of unwieldy code here:
            # So if USE_LOCAL and USE_REMOTE are both true, we do magic:
            # We iterate through both arrays simulanousley
            # If both are true, we set that to be rainbow
            # Else we darken it
            # To incorporate possibly white leds for visible presence without overlay, we should do an OR check before first
            if self.USE_LOCAL == True and self.USE_REMOTE == True:
                for idx, (ldi, rdi) in zip(self.local_difference_image, self.remote_difference_image):
                    if ldi == "1" and rdi == "1":
                        self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, randrange(255), randrange(255), randrange(255))
                    else:
                        self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 0, 0, 0)

            self.offscreen_canvas = self.matrix.SwapOnVSync(self.offscreen_canvas)

    def display_difference_image(self, difference_bool_array: str, mode = 0):
        if mode == 0:
            for idx, x in enumerate(difference_bool_array):
                if x == "1":
                    self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 255, 255, 255)
                else:
                    self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 0, 0, 0)
        
        if mode == 1:
            for idx, x in enumerate(difference_bool_array):
                if x == "1":
                    self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, randrange(255), randrange(255), randrange(255))
                else:
                    self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 0, 0, 0)
    
    def bytes_to_boolean_array(self, data_bytes: bytes):
        return str(data_bytes, 'UTF-8')

    def get_camera_difference_image(self, url: str):
        with urllib.request.urlopen(url) as response:
            return self.bytes_to_boolean_array(response.read())

if __name__ == "__main__":
    ripples_display = RipplesDisplay()
    if (not ripples_display.process()):
        ripples_display.print_help()
