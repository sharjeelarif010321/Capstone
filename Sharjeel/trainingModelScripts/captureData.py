# This code captures 50 frames from the default camera and saves them as JPEG images in a specified directory. The directory path is set in the save_directory variable, and the code creates the directory if it doesn't already exist. 

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 22th March 2023
###################################

# REVISION HISTORY

# 22 MAR 23: Developed script to automate capturing images. 

# 24 Mar 23: Changed from 75 images per run to 50. 

import os
import cv2

# set the directory path where the images will be saved
save_directory = "./dataset/lyingOnBelly"

# create the directory if it doesn't exist
if not os.path.exists(save_directory):
    os.makedirs(save_directory)

# get the number of existing images in the directory
existing_files = os.listdir(save_directory)
image_number = len(existing_files) + 1

# initialize the camera
cap = cv2.VideoCapture(0)

# loop through 50 frames
for i in range(50):
    # capture a frame
    ret, frame = cap.read()

    # display the frame
    cv2.imshow("Capture Frame", frame)

    # wait for a key press
    key = cv2.waitKey(1)

    # if the 'q' key is pressed, exit the loop
    if key == ord("q"):
        break

    # save the image with a numbered label that doesn't already exist in the directory
    while True:
        filename = f"{save_directory}/lying_on_belly{image_number}.jpg"
        if not os.path.exists(filename):
            break
        image_number += 1
    cv2.imwrite(filename, frame)

    # increment the image number
    image_number += 1

# release the camera and close all windows
cap.release()
cv2.destroyAllWindows()
