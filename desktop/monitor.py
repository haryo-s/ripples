import cv2
import socket

cv2.namedWindow("Receiver")

RECEIVER_UDP_IP = "127.0.0.1"
RECEIVER_UDP_PORT = 61252

# UDP Packet structure from Ripples
# 16 bytes: binary array (for neoflash hat only last 126 bits required)

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)
sock.bind((RECEIVER_UDP_IP, RECEIVER_UDP_PORT))

def bytes_to_boolean_array(data_bytes: bytes):
    # Convert bytes into binary representation
    binary_string = ''.join(format(byte, '08b') for byte in data_bytes)

    # Create a boolean array based on the binary representation
    boolean_array = [bit == '1' for bit in binary_string]

    return boolean_array

while True:
    packet, addr = sock.recvfrom(16)

    print(packet)
    print(type(packet))

    boolean_array = bytes_to_boolean_array(packet)
    print(boolean_array)

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

cv2.destroyWindow("Receiver")