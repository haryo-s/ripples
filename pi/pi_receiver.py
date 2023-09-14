from samplebase import SampleBase
import urllib.request

PANEL_WIDTH = 64
PANEL_HEIGHT = 32
URL = 'http://192.168.178.22:5000'

def bytes_to_boolean_array(data_bytes: bytes):
    # Convert bytes into binary representation
    binary_string = ''.join(format(byte, '08b') for byte in data_bytes)

    # Create a boolean array based on the binary representation
    boolean_array = [bit == '1' for bit in binary_string]

    return boolean_array

class RipplesDisplay(SampleBase):
    def __init__(self, *args, **kwargs):
        super(RipplesDisplay, self).__init__(*args, **kwargs)

    def run(self):
        canvas = self.matrix.CreateFrameCanvas()
        while True:
            with urllib.request.urlopen(URL) as response:
                print("Got a response")
                boolean_array = bytes_to_boolean_array(response.read())
                print(str(len(boolean_array)))

                if len(boolean_array) == PANEL_WIDTH*PANEL_HEIGHT:
                    print("Array is correct length")
                    for i in boolean_array:
                        if i == True:
                            print("True!")
                            canvas.SetPixel(i % PANEL_HEIGHT, i / PANEL_WIDTH, 255, 255, 255)
                        else:
                            print("Not true!")
                            canvas.SetPixel(i % PANEL_HEIGHT, i / PANEL_WIDTH, 0, 0, 0)

    # int coord[2];
    # coord[0] = i % 96; //160 is referring to width
    # coord[1] = i / 96;

if __name__ == "__main__":
    ripples_display = RipplesDisplay()
    if (not ripples_display.process()):
        ripples_display.print_help()
