# This is a Python script that implements a solution for a "safe snooze" system, which is designed to monitor a user's sleep and wake them up gently if necessary. The script uses various libraries and APIs to accomplish this goal. The script initializes and starts a video capture process, and then uses multiprocessing to handle the processing of frames in parallel. It also includes a function to read data from a serial connection, which is used to monitor the user's heart rate and blood oxygen levels. The script uses pyrebase to interface with a Firebase Realtime Database, where it stores data about the user's sleep, heart rate, blood oxygen levels, and posture. Additionally, the script uses pygame to play white noise to the user during their sleep. Finally, the script defines a class for the safe snooze system, with functions for starting and releasing the system, as well as a destroy() function to close the serial connection.

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 02th February 2023
###################################

# Import required libraries
# Some of these libraries need to be installed prior to use
import cv2, time
import os
os.environ['SDL_AUDIODRIVER'] = 'alsa'
os.environ['AUDIODEV'] = 'plughw:CARD=Headphones,DEV=0'

#time.sleep(10)

import mediapipe as mp
from multiprocessing import Process, Queue, Pool, Value, Array
from threading import Thread
from scipy.spatial import distance as dis

import pygame.mixer
pygame.mixer.init()

import alsaaudio

import serial
import RPi.GPIO as GPIO
from threading import Thread

from definitions import *
from calcFunctions import *

timestamp = str(int(time.time()))

# Add the serial connection setup code
ser = serial.Serial(port="/dev/ttyAMA1",
                    baudrate=9600,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS)
if (ser.isOpen() == False):
    ser.open()
ser.flushInput()
ser.flushOutput()

def echotest(heartrate, spo2, position, hr_sp_pos_queue):
    ser.flushInput()
    sp = '000'
    hr = '000'
    while (True):  # loop
        if (ser.in_waiting > 0):
            rx_data = ser.readline().decode('ascii', errors='ignore')
            #print('data received:', rx_data)
            
            # Check if the first 7 characters are spaces
            if rx_data[:7].isspace():
                # Replace the first 7 spaces with 0's
                rx_data = '000 000' + rx_data[7:]
                print("Updated rx_data:", rx_data)
            
            # Extract sp, hr, and pos data from rx_data
            if rx_data[:7] != '000 000':
                sp = rx_data[:3]
                hr = rx_data[4:7]

            pos = rx_data[8:36]
            
            #print(f"SP: {sp}, HR: {hr}, Pos: {pos}")

            # Only update shared memory variables if both hr and sp are not "000"
            #if hr != "000" or sp != "000":
            with heartrate.get_lock():
                heartrate.raw = hr.ljust(10, '\0').encode('ascii')
            with spo2.get_lock():
                spo2.raw = sp.ljust(10, '\0').encode('ascii')
            with position.get_lock():
                position.raw = pos.ljust(10, '\0').encode('ascii')
            
            # Put the updated values in the hr_sp_pos_queue
            hr_sp_pos_queue.put((hr, sp, pos))
            
            ser.flushInput()  # Clear UART Input buffer

# Add the destroy() function
def destroy():
    ser.close()
    print("test complete")

# Define the Class
class safeSnooze:
    # Initialize video capture vidSRC is default camera = 0
    def __init__(self, vidSrc = 0): 
        self.cap = cv2.VideoCapture(vidSrc)
        # Set buffer size to 5
        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 5)

        # Queue to share frames and processed frames between processes
        self.frame_queue = Queue()
        self.results_queue = Queue()
        # Initialize Process for process_frame function
        # Add a new Queue for sharing status and posture values between processes
        self.status_posture_queue = Queue()
        
        # Add shared memory variables
        self.heartrate = Array('b', b'111' * 10)
        self.spo2 = Array('b', b'111' * 10)
        self.position = Array('b', b'0' * 30)
        
        # Add the new hr_sp_pos_queue attribute
        self.hr_sp_pos_queue = Queue()

        # Include the new Queue in the process_frame function call
        self.proc = Process(target = process_frame, args = (self.frame_queue, self.results_queue, self.status_posture_queue))
    
    def start(self):
        # Start vid capture 
        self.proc.start()
        # Check to see if camera is opened and reading 
        while not self.cap.isOpened():
            pass

        #print("Waiting for 5 seconds before starting processing...")
        #time.sleep(5)
        # Enables 4 processes for input queue
        # One process should be able to handle most frames in time prior
        # to getting next frame from queue
        # 3 more threads are reserved incase processing times are delayed 
        # and more power is needed
        with Pool(processes = 2) as pool:
            while True:
                ret, frame = self.cap.read()
                if not ret:
                    break
                self.frame_queue.put(frame)
                processed_frame = self.results_queue.get()
                
                # Shows current frame
                
                #cv2.imshow("Frame", processed_frame)
                #if cv2.waitKey(1) & 0xFF == ord('q'):
                #    break
                
        # Adds None pointer in queue to stop processing
        self.frame_queue.put(None)
        # Waits for process frame to finish computing prior to moving forward
        # Allows for smoother flow
        self.proc.join()

    def release(self):
        # Releases camera
        self.cap.release()
        # Destroys frame window
        
        #cv2.destroyAllWindows()

import pyrebase

config = {
    "apiKey": "AIzaSyCLLkVheTK2jjsfLCVKc2UIAlEXGL4TFF8",
    "authDomain": "capstone-4d68a.firebaseapp.com",
    "databaseURL": "https://capstone-4d68a-default-rtdb.firebaseio.com",
    "projectId": "capstone-4d68a",
    "storageBucket": "capstone-4d68a.appspot.com",
    "messagingSenderId": "856828580524",
    "appId": "1:856828580524:web:564a79870ac9e0cfa30509"
}

firebase = pyrebase.initialize_app(config)
storage = firebase.storage()
database = firebase.database()

def update_database_status_posture(status_posture_queue):
    while True:
        status, posture = status_posture_queue.get()
        
        # Print the values received from the queue
        print(f"Status: {status}, Posture: {posture}")
        
        # Update the Firebase database with the new values
        update_database(status, posture)
        time.sleep(0.05)

def update_database_hr_sp_pos(hr_sp_pos_queue):
    while True:
        hr, sp, pos = hr_sp_pos_queue.get()
        
        # Print the values received from the queue
        print(f"Heart rate: {hr}, SpO2: {sp}, Position: {pos}")
        
        # Update the Firebase database with the new values
        update_database(None, None, hr, sp, pos)
        time.sleep(0.05)

def update_database(status=None, posture=None, heartrate=None, spo2=None, position=None):
    try:
        if status is not None and posture is not None:
            database.child(timestamp).child("Status").set({"Subject is": status})
            database.child(timestamp).child("Posture").set({"Subject is": posture})
        if heartrate is not None and spo2 is not None and position is not None:
            database.child(timestamp).child("Heartrate").set({"BPM": heartrate})
            database.child(timestamp).child("SpO2").set({"Percentage": spo2})
            database.child(timestamp).child("Position").set({"Value": position})
    except Exception as e:
        print(f"Error updating database: {e}")

def play_audio():
    #time.sleep(120)
    while(True):
        pygame.mixer.music.set_volume(0.6)
        print("volume at 60%")

        
        pygame.mixer.music.load('/home/raspberry/Desktop/Capstone/whiteNoise.mp3')
        pygame.mixer.music.play()

        print("playing audio")

        time.sleep(10)
        pygame.mixer.music.set_volume(0.8)
        print("volume at 80%")
        time.sleep(5)
        
        time.sleep(10)
        pygame.mixer.music.set_volume(1)
        print("volume at 100%")
        time.sleep(5)

        pygame.mixer.music.stop()
        
        time.sleep(5)
        

if __name__ == "__main__":
    link = safeSnooze()

    # Start the update_database_status_posture() function in a separate process
    update_status_posture_proc = Process(target=update_database_status_posture, args=(link.status_posture_queue,))
    update_status_posture_proc.start()

    # Start the update_database_hr_sp_pos() function in a separate process
    update_hr_sp_pos_proc = Process(target=update_database_hr_sp_pos, args=(link.hr_sp_pos_queue,))
    update_hr_sp_pos_proc.start()

    # Start the echotest() function in a separate process
    echotest_proc = Process(target=echotest, args=(link.heartrate, link.spo2, link.position, link.hr_sp_pos_queue))
    echotest_proc.start()

    play_audio_thread = Thread(target=play_audio)
    play_audio_thread.start()

    link.start()
    link.release()

    link.start()
    link.release()

    # Terminate the echotest process, update_status_posture_proc, and update_hr_sp_pos_proc after finishing the main loop
    echotest_proc.terminate()
    echotest_proc.join()

    update_status_posture_proc.terminate()
    update_status_posture_proc.join()

    update_hr_sp_pos_proc.terminate()
    update_hr_sp_pos_proc.join()

    # Call the destroy function to close the serial connection
    destroy()
