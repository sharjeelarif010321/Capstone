# This code creates training, validation, and test subsets of image data from a given dataset directory using the specified proportions. It creates the directories for each subset if they do not already exist, and defines the class names and size of the subsets

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 08th March 2023
###################################

# REVISION HISTORY

# 08 MAR 23: Created file to split the data into train, test, validation direcotry. 

# 23 Mar 23: Changed percentage of images to allocate to train = 80%, test = 10%, validation = 10% from train = 60%, test = 20%, validation = 20%

import os
import shutil
import random

dataset_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel/dataset'

# Define the paths to the training, validation, and test directories
train_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel/dataset/train'
val_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel/dataset/val'
test_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel/dataset/test'

# Create directories for training, validation, and testing subsets
os.makedirs(train_dir, exist_ok=True)
os.makedirs(val_dir, exist_ok=True)
os.makedirs(test_dir, exist_ok=True)

# Define the class names
class_names = ['lyingOnBack', 'lyingOnBelly', 'lyingOnSide', 'sittingDown']

# Define the size of the subsets
train_size = 0.8
val_size = 0.1
test_size = 0.1

# Loop over each class and split the images into the subsets
for class_name in class_names:
    class_dir = os.path.join(dataset_dir, class_name)
    file_names = os.listdir(class_dir)
    random.shuffle(file_names)
    num_files = len(file_names)
    train_end = int(num_files * train_size)
    val_end = int(num_files * (train_size + val_size))
    
    # Copy images to the appropriate subset directories
    for i, file_name in enumerate(file_names):
        src = os.path.join(class_dir, file_name)
        if i < train_end:
            dst = os.path.join(train_dir, class_name, file_name)
        elif i < val_end:
            dst = os.path.join(val_dir, class_name, file_name)
        else:
            dst = os.path.join(test_dir, class_name, file_name)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(src, dst)
