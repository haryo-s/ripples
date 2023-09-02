import cv2
import socket
import numpy

UDP_IP = "127.0.0.1"
UDP_PORT = 61252

# UDP_IP = "192.168.178.21"
# UDP_PORT = 8888

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)

vc = cv2.VideoCapture(0)

if vc.isOpened(): # try to get the first frame
    # First set both frames 
    rval, frame1 = vc.read()
    frame2 = frame1
else:
    rval = False

while rval:
    # Ok now we load frame2
    rval, frame2 = vc.read()
    
    # For diff, we subtract frame1 from frame2 and absolute the values
    diff_image = cv2.absdiff(cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY))

    frame1 = frame2

    scale_percent = 25
    end_width = int(diff_image.shape[1] * scale_percent / 100)
    end_height = int(diff_image.shape[0] * scale_percent / 100)
    dimensions = (end_width, end_height)

    diff_image_resized = cv2.resize(diff_image, (64, 64), interpolation = cv2.INTER_AREA)

    sendbuffer = ""

    for x in diff_image_resized:
        for y in x:
            if y > 30:
                sendbuffer += str(1)
            else:
                sendbuffer += str(0)


    data = "Hello from Python"
    data_string = bytes(sendbuffer, 'utf-8')

    sock.sendto(data_string, (UDP_IP, UDP_PORT))

    # Clear the buffer
    sendbuffer = ""

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()