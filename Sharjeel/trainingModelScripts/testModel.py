# This code evaluates a TensorFlow Lite model for image classification on a test dataset. It loads the TFLite model and defines the image size and image augmentation settings. The code then loops through each subdirectory in the test directory, loads and augments each image, and predicts the posture using the TFLite model. The predicted class is compared to the subdirectory name to calculate the accuracy of the model, which is logged to a file and printed to the console. The inference time for each image is also printed.

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 08th March 2023
###################################

# REVISION HISTORY

# 24 MAR 23: Create script to test model. 
# Script imports model and tests the model based on the images in the test directory

# 25 MAR 23: Added augmentation to images prior to test to randomize test

# 28 Mar 23: Added functionality to calculate accuracy of how well the model did

# 29 Mar 23: Calculating accuracy of model broken, fixing

# 29 Mar 23: Implemented chain to track correct class by using file name (track by count). Then use that count with count for incorrect image. 
# Still didn't work

# 29 MAR 23: Identified issue was with class naming convention not the same as files stored in. Changed to account for this
# Working correctly


import tensorflow as tf
import time
import numpy as np
import os
from PIL import Image
import re
from tensorflow.keras.preprocessing.image import ImageDataGenerator


# Load the TFLite model
model_path = "baby_posture_modelv4.tflite"
interpreter = tf.lite.Interpreter(model_path=model_path)
interpreter.allocate_tensors()

# Define the image size
img_size = (224, 224)

# Define the image augmentation settings
datagen = ImageDataGenerator(#rotation_range=30, 
                             #width_shift_range=0.2,
                             #height_shift_range=0.2, 
                             #shear_range=0.1,
                             #zoom_range=0.2,
                             #vertical_flip=True, 
                             #horizontal_flip=True,
                             fill_mode='nearest')

# Get the test directory path and classes
test_dir = "dataset/test"
test_classes = os.listdir(test_dir)

# Create a log file to store the evaluation results
log_file = open("evaluation_log_test5_TFLite.txt", "w")

# Initialize variables for accuracy calculation
total_images = 0
correct_predictions = 0

# Loop through each subdirectory in the test directory and evaluate the model
for sub_dir in test_classes:
    log_file.write(f"Evaluating images in {sub_dir} directory...\n")
    sub_dir_path = os.path.join(test_dir, sub_dir)
    for image_file in os.listdir(sub_dir_path):
        image_path = os.path.join(sub_dir_path, image_file)
        print(f"Evaluating image: {image_file}")
        
        # Remove the digits from the end of the file name
        image_file_no_digits = re.sub(r'\d+$', '', image_file)
        
        # Load and augment the image
        img = Image.open(image_path)
        img = img.resize(img_size)
        img_arr = np.array(img)
        img_arr = np.expand_dims(img_arr, axis=0)
        aug_iter = datagen.flow(img_arr)
        aug_img = next(aug_iter)[0].astype('float32') / 255.0
        aug_img = np.expand_dims(aug_img, axis=0)
        
        # Predict the posture using the TFLite model
        input_details = interpreter.get_input_details()
        output_details = interpreter.get_output_details()

        start_time = time.time() # start time
        interpreter.set_tensor(input_details[0]['index'], aug_img)
        interpreter.invoke()
        result = interpreter.get_tensor(output_details[0]['index'])
        end_time = time.time() # end time

        inference_time_ms = (end_time - start_time) * 1000.0 # inference time in milliseconds
        
        predicted_class_index = np.argmax(result[0])
        class_map = {0: 'lyingOnBack', 1: 'lyingOnBelly', 2: 'lyingOnSide', 3: 'sittingDown'}
        predicted_class = class_map[predicted_class_index]

        print(f"Predicted class: {predicted_class}")
        print(f"Inference time: {inference_time_ms:.2f} ms") # print the inference time
        print(f"Subdirectory: {sub_dir}")
        
        log_file.write(f"{image_file} is predicted to be {predicted_class}\n")
        
        # Remove the digits from the end of the subdirectory name
        sub_dir_no_digits = re.sub(r'\d+$', '', sub_dir)
        
        # Check if the predicted class matches the subdirectory name
        if predicted_class.lower() == sub_dir_no_digits.lower() and image_file_no_digits.lower() == image_file.lower():
            correct_predictions += 1
        total_images += 1

# Calculate the accuracy percentage
accuracy = (correct_predictions / total_images) * 100

# Write the accuracy to the log file and print it to the console
log_file.write(f"Accuracy: {accuracy:.2f}% ({correct_predictions}/{total_images} images)\n")
print(f"Accuracy: {accuracy:.2f}% ({correct_predictions}/{total_images} images)\n")
log_file.close()
