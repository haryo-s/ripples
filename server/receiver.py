import cv2
import socket
import numpy

cv2.namedWindow("Receiver")

UDP_IP = "127.0.0.1"
UDP_PORT = 61252

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    string_data, addr = sock.recvfrom(20000)
    data = numpy.fromstring(string_data, dtype='uint8')
    print(data)
    decoded_image = cv2.imdecode(data,1)
    cv2.imshow("Receiver", decoded_image)

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

cv2.destroyWindow("preview")