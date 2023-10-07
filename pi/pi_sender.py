from flask import Flask, render_template, Response
import cv2

app = Flask(__name__)

LEDPANEL_DIMENSIONS = (64, 32)
DIFF_THRESHOLD = 32

send_buffer = ""

camera = cv2.VideoCapture(0)  # use 0 for web camera
#camera = cv2.VideoCapture(-1)
#camera = cv2.VideoCapture('/dev/video0')
# camera = cv2.VideoCapture('/dev/video0', apiPreference=cv2.CAP_V4L2)

def get_diff_frame():
    return_val, frame1 = camera.read()
    frame2 = frame1

    diff_frame = ""
    
    if return_val:
        return_val, frame1 = camera.read()
        diff_image = cv2.absdiff(cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY))

        diff_image_resized = cv2.resize(diff_image, LEDPANEL_DIMENSIONS, interpolation = cv2.INTER_AREA)

        for x in diff_image_resized:
            for y in x:
                if y > DIFF_THRESHOLD:
                    diff_frame += str(1)
                else:
                    diff_frame += str(0)
    return diff_frame

@app.route('/')
def index():
    return Response("Hello!", mimetype='text/plain')

@app.route('/updatebuffer')
def update_buffer():
    send_buffer = get_diff_frame()
    return Response(send_buffer, mimetype='text/plain')

@app.route('/sendbuffer')
def send_buffer():
    return Response(send_buffer, mimetype='text/plain')

@app.route('/test')
def test():
    ret, frame = camera.read()
    print(ret, frame)
    return Response(str(ret))

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, use_reloader=False)
