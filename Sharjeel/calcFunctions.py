import cv2
from definitions import *
import mediapipe as mp
from multiprocessing import Process, Queue
from scipy.spatial import distance as dis
import cProfile

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

# Uses the posture landmarks to calculate anges between two different poitns
# These angles are then used to determine the posture of the subject
def getPosture(image, outputs):
    
    # Extract the landmarks of the relevant body parts
    left_shoulder = outputs.pose_landmarks.landmark[LEFT_SHOULDER]
    right_shoulder = outputs.pose_landmarks.landmark[RIGHT_SHOULDER]
    left_hip = outputs.pose_landmarks.landmark[LEFT_HIP]
    right_hip = outputs.pose_landmarks.landmark[RIGHT_HIP]
    left_ankle = outputs.pose_landmarks.landmark[LEFT_ANKLE]
    right_ankle = outputs.pose_landmarks.landmark[RIGHT_ANKLE]
    left_elbow = outputs.pose_landmarks.landmark[LEFT_ELBOW]
    right_elbow = outputs.pose_landmarks.landmark[RIGHT_ELBOW]
    left_knee = outputs.pose_landmarks.landmark[LEFT_KNEE]
    right_knee = outputs.pose_landmarks.landmark[RIGHT_KNEE]
    
    # Calculate the angles between the landmarks
    shoulder_hip_angle = eucDistance(image, left_shoulder, right_shoulder)
    hip_ankle_left_angle = eucDistance(image, left_hip, left_ankle)
    hip_ankle_right_angle = eucDistance(image, right_hip, right_ankle)
    elbow_knee_left_angle = eucDistance(image, left_elbow, left_knee)
    elbow_knee_right_angle = eucDistance(image, right_elbow, right_knee)
    
    # Classify the posture based on the angles
    posture = "unknown"
    if shoulder_hip_angle > 170 and shoulder_hip_angle < 190:
        posture = "lying on back"
    elif hip_ankle_left_angle > 90 and hip_ankle_right_angle > 90 and elbow_knee_left_angle > 90 and elbow_knee_right_angle > 90:
        posture = "lying on side"
    elif shoulder_hip_angle < 80:
        posture = "lying on belly"
    elif elbow_knee_left_angle < 70 and elbow_knee_right_angle < 70:
        posture = "crouching"
    else:
        posture = "sitting"
    return posture

# Takes in frame from queue processes the frame by first drawing landmarks on face
# and pose landmarks on the body. It then calls eucDistance to get aspect ratio
# between the top and bottm of eyelids. If 
def process_frame(frame_queue, results_queue):
    #Adding these variables globally within this function to avoid errors.
    global ratio, status, previousStatus, previousPostureStatus, frameCount
    with mp_holistic.Holistic(min_detection_confidence = minDetectionConfidence) as holistic:
        while True:
            # Gets frame from frame_queue 
            frame = frame_queue.get()
            # If no frame is detected wait until frame is in queue 
            if frame is None:
                break
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

                mp_drawing.draw_landmarks(image, results.face_landmarks, mp_facemesh.FACEMESH_LEFT_EYE, 
                               mp_drawing.DrawingSpec(color = GREEN, thickness=1, circle_radius=1),
                               mp_drawing.DrawingSpec(color= LGREEN, thickness=1, circle_radius=1)
                               )
                mp_drawing.draw_landmarks(image, results.face_landmarks, mp_facemesh.FACEMESH_RIGHT_EYE, 
                               mp_drawing.DrawingSpec(color = GREEN, thickness=1, circle_radius=1),
                               mp_drawing.DrawingSpec(color= LGREEN, thickness=1, circle_radius=1)
                               )

                mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_holistic.POSE_CONNECTIONS, 
                                mp_drawing.DrawingSpec(color = ORANGE, thickness=1, circle_radius=4),
                                mp_drawing.DrawingSpec(color = PURPLE, thickness=1, circle_radius=2)
                                )                   
            else: 
                ratio = 0

            # Uses calculation functions to calculate if subject is asleep, awake
            # and their posture
            if results.pose_landmarks is not None:
                posture = getPosture(image, results)
                if posture != previousPostureStatus:
                    previousPostureStatus = posture
                    print("Subject posture is:", posture)

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

            results_queue.put(image)