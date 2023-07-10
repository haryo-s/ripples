import cv2
import socket
import numpy

TARGET_UDP_IP = "127.0.0.1"
TARGET_UDP_PORT = 61252

RECEIVE_UDP_IP = "127.0.0.1"
RECEIVE_UDP_PORT = 50000

target_sock = socket.socket(socket.AF_INET,
                            socket.SOCK_DGRAM)

receive_sock = socket.socket(socket.AF_INET,
                             socket.SOCK_DGRAM)

# Creating preview windows for the send and receive
cv2.namedWindow("Send")
cv2.namedWindow("Receive")

# Establishing videocapture
vc = cv2.VideoCapture(0)

if vc.isOpened(): # try to get the first frame
    # First set both frames 
    rval, frame1: numpy.ndarray = vc.read()
    frame2: numpy.ndarray = frame1
    print(type(frame1))
else:
    rval = False

while rval:
    # Fist part: We read the image, get the diff and we send it off to the other node

    # Ok now we load frame2
    rval, frame2 = vc.read()
    
    # For diff, we subtract frame1 from frame2 and absolute the values
    diff_image = cv2.absdiff(cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY))

    frame1 = frame2

    scale_percent = 25
    end_width = int(diff_image.shape[1] * scale_percent / 100)
    end_height = int(diff_image.shape[0] * scale_percent / 100)
    dimensions = (end_width, end_height)

    diff_image_resized = cv2.resize(diff_image, dimensions, interpolation = cv2.INTER_AREA)

    cv2.imshow("Send", diff_image_resized)

    encode_parameters = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
    result, img_encoded = cv2.imencode(".jpg", diff_image_resized, encode_parameters)
    data: numpy.ndarray = numpy.array(img_encoded)
    data_string = data.tobytes()

    target_sock.sendto(data_string, (TARGET_UDP_IP, TARGET_UDP_PORT))

    # Second part: We receive the image from the other node, decode it and display it
    string_data, addr = receive_sock.recvfrom(20000)
    data = numpy.fromstring(string_data, dtype='uint8')
    decoded_image = cv2.imdecode(data,1)
    cv2.imshow("Receive", decoded_image)

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow("Send")
cv2.destroyWindow("Receive")