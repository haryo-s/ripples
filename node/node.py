import cv2
import socket
import numpy

# UDP_IP = "127.0.0.1"
# UDP_PORT = 61252

UDP_IP = "192.168.178.21"
UDP_PORT = 8888

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)

cv2.namedWindow("preview")
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

    diff_image_resized = cv2.resize(diff_image, dimensions, interpolation = cv2.INTER_AREA)

    cv2.imshow("preview", diff_image_resized)

    encode_parameters = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
    result, img_encoded = cv2.imencode(".jpg", diff_image_resized, encode_parameters)
    # data: numpy.ndarray = numpy.array(img_encoded)
    data = "Hello from Python"
    data_string = bytes(data, 'utf-8')

    # sock.sendto(bytes(len(data_string)), (UDP_IP, UDP_PORT))
    sock.sendto(data_string, (UDP_IP, UDP_PORT))

    # sock.sendto(diff_image_resized, (UDP_IP, UDP_PORT))

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow("preview")