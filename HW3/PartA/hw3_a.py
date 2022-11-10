import numpy as np
import cv2 as cv
import glob
import os
from tqdm import tqdm 

cameraName = "phone" # fv5 or phone

# termination criteria
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)
# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
size = np.array([8, 6])
objp = np.zeros((size[0]*size[1], 3), np.float32)
objp[:,:2] = np.mgrid[0:size[0], 0:size[1]].T.reshape(-1,2)
# Arrays to store object points and image points from all the images
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane

images = glob.glob(f'./{cameraName}_images/*.jpg')
for fname in images:
    print(f"now at {fname}")
    img = cv.imread(fname)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    # Find the chess board corners
    ret, corners = cv.findChessboardCorners(gray, size, None)

    # If found, add object points, image points (after refining them)

    if ret != True:
        print("found no corners!")
    else:
        objpoints.append(objp)
        corners2 = cv.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
        imgpoints.append(corners2)
        print("==============================")

ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
print('camera matrix:\n{}'.format(mtx))

print("<Now do the calibration>")
for fname in tqdm(images):
    # img = cv.imread(f'{cameraName}_original.jpg')
    img = cv.imread(fname)
    h,  w = img.shape[:2]
    newcameramtx, roi = cv.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))

    # undistort
    dst = cv.undistort(img, mtx, dist, None, newcameramtx)
    # crop the image
    x, y, w, h = roi
    dst = dst[y:y+h, x:x+w]
    name = os.path.basename(fname)
    cv.imwrite(f'./calib_images/calib_{name}', dst)