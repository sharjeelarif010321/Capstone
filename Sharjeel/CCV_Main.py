# ###################################################
# SafeSnooze - Computational Computer Vision Code
# This code is the foundation for the computational
# computer vision aspect of the project. 
#
# This code utilizes multiple libraries including:
# cv2, mediapipe, multiprocessing, scipy
# These libraries are utilized and make up the bulk
# of the code. The code has 4 main aspects to it
#
# Part 1) Using openCV to get frames from camera
# These frames are stored in a queue to separate 
# this part of the project from the other parts.
#
# Part 2) The frames are collected from the queue
# and are processed using the processing function
# this function is responsible for drawing landmarks
# on the body of the subject and using those landmarks
# to calculate different states the subject is in
#
# Part 3) The processed frames are then stored in a
# different queue and outputted to show the user
# the state variables are also updated if and only if
# the state changes

# Import required libraries
# Some of these libraries need to be installed prior to use
import cv2, time
import mediapipe as mp
from multiprocessing import Process, Queue, Pool
from scipy.spatial import distance as dis
import numpy as np

from definitions import *
from calcFunctions import *

# Define the Class
class safeSnooze:
    # Initialize video capture vidSRC is default camera = 0
    def __init__(self, vidSrc = 0): 
        self.cap = cv2.VideoCapture(vidSrc)
        # Set buffer size to 5
        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 5)

        self.cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')) # depends on fourcc available camera
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 2560)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1440)
        self.cap.set(cv2.CAP_PROP_FPS, 5)
        # Get the current fps
        fps = self.cap.get(cv2.CAP_PROP_FPS)

        # Print the fps
        print("Current fps:", fps)

        # Queue to share frames and processed frames between processes
        self.frame_queue = Queue()
        self.results_queue = Queue()
        # Initialize Process for process_frame function
        self.proc = Process(target = process_frame, args = (self.frame_queue, self.results_queue))
    
    def start(self):
        # Start vid capture 
        self.proc.start()
        # Check to see if camera is opened and reading 
        while not self.cap.isOpened():
            pass

        # Enables 4 processes for input queue
        # One process should be able to handle most frames in time prior
        # to getting next frame from queue
        # 3 more threads are reserved incase processing times are delayed 
        # and more power is needed
        with Pool(processes = 1) as pool:
            while True:
                ret, frame = self.cap.read()
                if not ret:
                    break
                self.frame_queue.put(frame)
                processed_frame = self.results_queue.get()
                
                # Shows current frame
                cv2.imshow("Frame", processed_frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
        # Adds None pointer in queue to stop processing
        self.frame_queue.put(None)
        # Waits for process frame to finish computing prior to moving forward
        # Allows for smoother flow
        self.proc.join()

    def release(self):
        # Releases camera
        self.cap.release()
        # Destroys frame window
        cv2.destroyAllWindows()


# Initializes safeSnooze sequence
if __name__ == "__main__":
    link = safeSnooze()
    link.start()
    link.release()

