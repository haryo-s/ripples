import cv2

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

    cv2.imshow("preview", diff_image)

    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow("preview")