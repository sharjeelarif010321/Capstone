###################################
# Author: Sharjeel Arif
# SID: 200388331
# 02th February 2023
###################################

import cv2
from definitions import *
import mediapipe as mp
from multiprocessing import Process, Queue
from scipy.spatial import distance as dis
import tensorflow as tf
import numpy as np


# Load the TFLite model
tflite_model_path = "/home/raspberry/Desktop/Capstone/baby_posture_modelv4.tflite"
interpreter = tf.lite.Interpreter(model_path=tflite_model_path)

interpreter._get_tensor_delegate = lambda x: (
    tf.lite.experimental.load_delegate('libedgetpu.so.1')
    if 'edgetpu' in x.name else None)

interpreter.allocate_tensors()

# Get input and output tensors
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Define the classes
classes = ["back", "belly", "side", "sitting"]

# Initialize drawing, holisitic, facemesh libraries
# as mp_drawing, mp_holistic, mp_facemesh from mediapipe library
mp_drawing = mp.solutions.drawing_utils
mp_holistic = mp.solutions.holistic
mp_facemesh = mp.solutions.face_mesh_connections

# Uses SciPy library to calculate distance between points
# SciPy is a C wrapper for Python which allows calculations to be done faster
# These calculations can become a big hinderence if I use python to calculate
# This is due to the fact that Python is interpretted during runtime 
# Whereas C/C++ is compiled at compile time
def eucDistance(image, top, bottom):
    height, width = image.shape[0:2]
            
    point1 = int(top.x * width), int(top.y * height)
    point2 = int(bottom.x * width), int(bottom.y * height)
    
    distance = dis.euclidean(point1, point2)
    return distance

# Uses eucDistance function to calculate ratios between points
def getAspectRatio(image, outputs, top_bottom, left_right):
    landmark = outputs.face_landmarks
            
    top = landmark.landmark[top_bottom[0]]
    bottom = landmark.landmark[top_bottom[1]]
    
    top_bottom_dis = eucDistance(image, top, bottom)
    
    left = landmark.landmark[left_right[0]]
    right = landmark.landmark[left_right[1]]
    
    left_right_dis = eucDistance(image, left, right)
    
    aspect_ratio = left_right_dis/ top_bottom_dis
    
    return aspect_ratio


def getPosture(frame, results):
    # Convert the frame to RGB
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Resize the frame to (150, 150)
    frame = cv2.resize(frame, (224, 224), interpolation=cv2.INTER_LINEAR)
    
    # Normalize the frame
    frame = frame.astype(np.float32) / 255.0
    
    # Expand the dimensions of the frame to match the input shape of the model
    frame = np.expand_dims(frame, axis=0)
    
    # Set the input tensor
    interpreter.set_tensor(input_details[0]['index'], frame)
    
    # Invoke the interpreter
    interpreter.invoke()

    # Get the prediction
    result = interpreter.get_tensor(output_details[0]['index'])

    thresholds = {'back': 0.65, 'belly': 0.35, 'side': 0.45, 'sitting': 0.35}

    if result[0][0] > thresholds['back'] and result[0][0] > result[0][1] and result[0][0] > result[0][2] and result[0][0] > result[0][3]:
        posture = 'Lying on back'
    elif result[0][1] > thresholds['belly'] and result[0][1] > result[0][0] and result[0][1] > result[0][2] and result[0][1] > result[0][3]:
        posture = 'Lying on belly'
    elif result[0][2] > thresholds['side'] and result[0][2] > result[0][0] and result[0][2] > result[0][1] and result[0][2] > result[0][3]:
        posture = 'Lying on side'
    else:
        posture = 'Sitting down'

    print("Predicted: ", posture)


    return posture

# Takes in frame from queue processes the frame by first drawing landmarks on face
# and pose landmarks on the body. It then calls eucDistance to get aspect ratio
# between the top and bottm of eyelids. If 
def process_frame(frame_queue, results_queue, status_posture_queue):
    global ratio, status, previousStatus, previousPostureStatus, frameCount, statusValue
    
    with mp_holistic.Holistic(min_detection_confidence=minDetectionConfidence) as holistic:
        frame_number = 0
        while True:
            # Gets frame from frame_queue 
            frame = frame_queue.get()
            # If no frame is detected wait until frame is in queue 
            if frame is None:
                break
            frame_number += 1
            # Changes frame to black and white and processes holistic landmarks
            # Then changes the frame back to color
            image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            results = holistic.process(image)
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

            # Draws landmarks on holistic processed frames
            # Calculates ratio of top of eye and bottom of eye of both eyes
            if results.face_landmarks is not None:
                ratio_left =  getAspectRatio(image, results, LEFT_EYE_TOP_BOTTOM, LEFT_EYE_LEFT_RIGHT)
                ratio_right =  getAspectRatio(image, results, RIGHT_EYE_TOP_BOTTOM, RIGHT_EYE_LEFT_RIGHT)

                ratio = (ratio_left + ratio_right)/2.0      

            else: 
                ratio = 0

            # Pass every 20th frame to the getPosture function
            if frame_number % 30 == 0:
                # Submit the task to the executor
                posture = getPosture(image, results)

            else:
                posture = previousPostureStatus

            previousPostureStatus = posture

            # Display the posture
            cv2.putText(image, f"Posture: {posture}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

            if ratio > minTolerance:
                frameCount +=1
            else:
                frameCount = 0
                
            if frameCount > minFrame:
                #Closing the eyes
                status = "asleep"

            else:
                status = "awake"

            if status != previousStatus:
                print("Subject is", status)
                previousStatus = status
                #statusValue = status

            # Update the shared variables statusValue and postureValue
            statusValue, postureValue = status, posture

            # Add the updated status and posture values to the queue
            status_posture_queue.put((statusValue, postureValue))


            results_queue.put(image)
