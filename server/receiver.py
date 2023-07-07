import cv2
import socket

# cv2.namedWindow("Receiver")

UDP_IP = "127.0.0.1"
UDP_PORT = 61252

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(20000)
    print(data)
