import cv2
import urllib.request
import numpy as np
import requests


def nothing(x):
    pass


url = 'http://172.20.10.5/cam-mid.jpg'
url_center = 'http://172.20.10.5/center'

cv2.namedWindow("live transmission", cv2.WINDOW_AUTOSIZE)

'''
cv2.namedWindow("Tracking")
cv2.createTrackbar("LH", "Tracking", 0, 255, nothing)
cv2.createTrackbar("LS", "Tracking", 0, 255, nothing)
cv2.createTrackbar("LV", "Tracking", 0, 255, nothing)
cv2.createTrackbar("UH", "Tracking", 255, 255, nothing)
cv2.createTrackbar("US", "Tracking", 255, 255, nothing)
cv2.createTrackbar("UV", "Tracking", 255, 255, nothing)
'''


def post_center(x, y):

    center = str(x) + " " + str(y)
    # Define the string payload
    payload = {'message': center}

    # Send the HTTP POST request with the string data
    response = requests.post(url_center, data=payload)

    # Check the response status
    if response.status_code == 200:
        print("String data sent successfully to the ESP32!")
    else:
        print("Error sending string data to the ESP32:", response.text)


while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    frame = cv2.imdecode(imgnp, -1)

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    '''
    l_h = cv2.getTrackbarPos("LH", "Tracking")
    l_s = cv2.getTrackbarPos("LS", "Tracking")
    l_v = cv2.getTrackbarPos("LV", "Tracking")

    u_h = cv2.getTrackbarPos("UH", "Tracking")
    u_s = cv2.getTrackbarPos("US", "Tracking")
    u_v = cv2.getTrackbarPos("UV", "Tracking")

    l_b = np.array([l_h, l_s, l_v])
    u_b = np.array([u_h, u_s, u_v])
    '''

    l_b = np.array([128, 128, 0])
    u_b = np.array([255, 255, 255])
    mask = cv2.inRange(hsv, l_b, u_b)

    contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # x = 800/2
    # y = 600/2
    for contour in contours:
        area = cv2.contourArea(contour)
        color = (0, 0, 255)
        if (area > 3000):
            cv2.drawContours(frame, [contour], -1, color, 3)
            M = cv2.moments(contour)
            x = int(M["m10"]/M["m00"])
            y = int(M["m01"]/M["m00"])
            frame = cv2.circle(frame, (x, y), 7, (255, 255, 255), 3)  # draw center
            print(x, y)                                # print center point
            post_center(x, y)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    cv2.imshow("live transmission", frame)
    # cv2.imshow("mask", mask)
    # cv2.imshow("res", res)
    key = cv2.waitKey(5)
    if key == ord('q'):
        break

cv2.destroyAllWindows()
