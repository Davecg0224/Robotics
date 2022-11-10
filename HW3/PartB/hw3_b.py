import numpy as np
import cv2 as cv
import glob
import os
import matplotlib.pyplot as plt
from tqdm import tqdm

def findCentroid(M):
    cx = int(M['m10']/M['m00'])
    cy = int(M['m01']/M['m00'])
    return cx, cy

def findLine(M, cx, cy):
    length = 500
    u11 = M['mu11']
    u20 = M['mu20']
    u02 = M['mu02']
    angle = np.arctan2(2*u11, (u20-u02)) / 2
    startPoint = (cx-length, cy-int(length*np.tan(angle)))
    endPoint = (cx+length, cy+int(length*np.tan(angle)))
    return angle, startPoint, endPoint

def drawAndSave(img):
    ori_img = img.copy()
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    ret, thresh = cv.threshold(gray, 190, 255, 0)
    contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

    for i in range(len(contours)):
        cnt = contours[i]
        M = cv.moments(cnt)
        area = M['m00']
        if area > areaThres:
            # draw the centroid
            cx, cy = findCentroid(M)
            cv.circle(ori_img,(cx,cy), 2, (255, 0, 0), 6)

            # draw the principal lines
            angle, startPoint, endPoint = findLine(M, cx, cy)
            cv.line(ori_img, startPoint, endPoint, (0, 0, 255), 2)
            # print('Centroid = {}\nPrinciple angle = {:.1f} deg'.format((cx, cy), np.degrees(angle)))
    
    plt.imshow(ori_img)
    plt.xlabel('Centroid = {}\nPrinciple angle = {:.1f}deg'.format((cx, cy), np.degrees(angle)))
    plt.savefig(f'./est_images/{name}')

if __name__ == "__main__":
    areaThres = 1.0
    images = glob.glob('./images/*.jpg')
    for image in tqdm(images):
        name = os.path.basename(image)
        # print(f"<Now at {name}>")
        img = cv.imread(image)
        drawAndSave(img)
        # print("==========================================")