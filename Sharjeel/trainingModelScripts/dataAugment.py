# This code is implementing image data augmentation for a machine learning model. It imports necessary libraries such as os, numpy, and tensorflow. It defines the image augmentation settings using the ImageDataGenerator class from Keras, which will apply random transformations to input images to generate new training data. Then, it defines a base directory and file structure for the images to be processed. The code iterates over each image category and applies the defined image augmentation to each image in the category directory. It saves five augmented images for each original image, and the augmented images are saved to the same directory as the original image.


###################################
# Author: Sharjeel Arif
# SID: 200388331
# 27th March 2023
###################################

# REVISION HISTORY

# 27 MAR 23: Created file to augment data from existing dataset

# 27 MAR 23: Modified file to add augmentation using TF Keras ImageDataGenerator

# 29 MAR 23: Modified augmentation weights in the ImageDataGenerator function

import os
import numpy as np
from tensorflow.keras.preprocessing.image import ImageDataGenerator, load_img, img_to_array

# Define image augmentation settings
datagen = ImageDataGenerator(
    rotation_range=25,
    width_shift_range=0.15,
    height_shift_range=0.15,
    shear_range=0.2,
    zoom_range=-0.3,
    vertical_flip=True,
    fill_mode='nearest'
)

# Define directory and file structure
base_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel//dataset'
categories = [
    ('lyingOnBack', 'lying_on_back'),
    ('lyingOnBelly', 'lying_on_belly'),
    ('lyingOnSide', 'lying_on_side'),
    ('sittingDown', 'sitting_down')
]

# Process images in each category
for category_dir, file_prefix in categories:
    dir_path = os.path.join(base_dir, category_dir)

    # Iterate over images in the category directory
    for file_name in os.listdir(dir_path):
        img_path = os.path.join(dir_path, file_name)
        img = load_img(img_path)
        x = img_to_array(img)
        x = np.expand_dims(x, axis=0)

        # Generate augmented images
        i = 0
        prefix = file_name[:-4]  # Remove file extension
        for batch in datagen.flow(x, batch_size=1, save_to_dir=dir_path, save_prefix=prefix, save_format='jpg'):
            i += 1
            if i >= 1:  # Save 5 augmented images for each original image
                break  # Exit the loop to stop generating more images
