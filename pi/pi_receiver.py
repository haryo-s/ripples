#!/usr/bin/env python

from samplebase import SampleBase
from random import randrange
import urllib.request
import time

class RipplesDisplay(SampleBase):
    def __init__(self, *args, **kwargs):
        super(RipplesDisplay, self).__init__(*args, **kwargs)

        self.parser.add_argument("--display-mode", action="store", help="Set display mode. Currently not implemented", default="rainbow", type=str)
        self.parser.add_argument("--sources", action="store", help="Set image sources. Uses a string. 'l' for local, 'r' for remote, 'lr' for both", default="lr", type=str)
        self.parser.add_argument("--remote-url", action="store", help="Set image sources. Uses a string. 'l' for local, 'r' for remote, 'lr' for both", default="http://192.168.178.22:5000", type=str)

        self.args = self.parser.parse_args()

        self.PANEL_WIDTH  = 64
        self.PANEL_HEIGHT = 32
        self.PANEL_LENGTH = self.PANEL_WIDTH * self.PANEL_HEIGHT
        self.URL_LOCAL    = 'http://127.0.0.1:5000'
        self.URL_REMOTE   = self.args.remote_url
        # self.URL_REMOTE   = 'http://192.168.178.22:5000'
        
        self.USE_LOCAL    = True
        self.USE_REMOTE   = True

    def run(self):
        self.offscreen_canvas = self.matrix.CreateFrameCanvas()
        self.width = self.offscreen_canvas.width
        self.height = self.offscreen_canvas.height

        self.local_difference_image = ""
        self.remote_difference_image = ""
        
        print("USE_LOCAL: " + str(self.USE_LOCAL))
        print("USE_REMOTE: " + str(self.USE_REMOTE))

        while True:
            time.sleep(1)
            # Getting image from local cam if USE_LOCAL is true
            if self.USE_LOCAL:
                try:
                    self.local_difference_image = self.get_camera_difference_image(self.URL_LOCAL)
                except:
                    print("Failed to receive local image")
                    pass # On failure pass and don't replace image
                else:
                    if len(self.local_difference_image) != self.PANEL_LENGTH:
                        pass # If length is incorrect don't replace image
            # Getting image from remote cam if USE_REMOTE is true
            if self.USE_REMOTE:
                try:
                    self.remote_difference_image = self.get_camera_difference_image(self.URL_REMOTE)
                except:
                    print("Failed to receive remote image")
                    pass # On failure pass and don't replace image
                else:
                    if len(self.remote_difference_image) != self.PANEL_LENGTH:
                        pass # If length is incorrect don't replace image

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
                for idx, (ldi, rdi) in enumerate(zip(self.local_difference_image, self.remote_difference_image)):
                    light_led = self.nand_boolean_array(ldi, rdi)
                    if ldi == "0" and rdi == "0": # Doing a check here already if both are 0
                        self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 0, 0, 0)
                    elif light_led == True:
                        self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 127, 127, 127)
                    elif light_led == False:
                        self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, randrange(255), randrange(255), randrange(255))

                    # if ldi == "1" or rdi == "1":
                    #     self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 255, 255, 255)
                    # if ldi == "1" and rdi == "1":
                    #     self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, randrange(255), randrange(255), randrange(255))
                    # else:
                    #     self.offscreen_canvas.SetPixel(idx % self.width, idx / self.width, 0, 0, 0)

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
    
    def nand_boolean_array(self, a, b):
        if a == "1" and b == "1":
            return False
        else:
            return True

    def bytes_to_boolean_array(self, data_bytes: bytes):
        return str(data_bytes, 'UTF-8')

    def get_camera_difference_image(self, url: str):
        with urllib.request.urlopen(url) as response:
            return self.bytes_to_boolean_array(response.read())

if __name__ == "__main__":
    ripples_display = RipplesDisplay()
    if (not ripples_display.process()):
        ripples_display.print_help()
