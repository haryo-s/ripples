from flask import Flask, render_template, Response
import cv2

app = Flask(__name__)

LEDPANEL_DIMENSIONS = (64, 32)
DIFF_THRESHOLD = 32

camera = cv2.VideoCapture(0)  # use 0 for web camera

def gen_frames():  # generate frame by frame from camera
    while True:
        # Capture frame-by-frame
        success, frame = camera.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
            
def get_diff_frame():
    return_val, frame1 = camera.read()
    frame2 = frame1

    if return_val:
        return_val, frame1 = camera.read()
        diff_image = cv2.absdiff(cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY))

        diff_image_resized = cv2.resize(diff_image, LEDPANEL_DIMENSIONS, interpolation = cv2.INTER_AREA)

        sendbuffer = ""

        for x in diff_image_resized:
            for y in x:
                if y > DIFF_THRESHOLD:
                    sendbuffer += str(1)
                else:
                    sendbuffer += str(0)
    
    return sendbuffer

@app.route('/video_feed')
def video_feed():
    #Video streaming route. Put this in the src attribute of an img tag
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/')
def index():
    sendbuffer = get_diff_frame()
    return sendbuffer


if __name__ == '__main__':
    app.run(debug=True)