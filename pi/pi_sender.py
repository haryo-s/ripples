from flask import Flask, render_template, Response
import cv2

app = Flask(__name__)

LEDPANEL_DIMENSIONS = (64, 32)
DIFF_THRESHOLD = 32

camera = cv2.VideoCapture(0)  # use 0 for web camera
            
def get_diff_frame():
    return_val, frame1 = camera.read()
    frame2 = frame1

    sendbuffer = ""

    if return_val:
        return_val, frame1 = camera.read()
        diff_image = cv2.absdiff(cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY))

        diff_image_resized = cv2.resize(diff_image, LEDPANEL_DIMENSIONS, interpolation = cv2.INTER_AREA)

        for x in diff_image_resized:
            for y in x:
                if y > DIFF_THRESHOLD:
                    sendbuffer += str(1)
                else:
                    sendbuffer += str(0)
    print(len(sendbuffer))
    return sendbuffer

@app.route('/')
def index():
    sendbuffer = get_diff_frame()
    return Response(sendbuffer, mimetype='text/plain')


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)