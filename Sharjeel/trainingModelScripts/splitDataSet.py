import os
import shutil
from sklearn.model_selection import train_test_split

# Define the path to the directory containing the labeled images
data_dir = '/Users/sharjeelarif/Desktop/CapstoneTrainingModel'

# Define the paths to the training, validation, and test directories
train_dir = 'data/train'
val_dir = 'data/val'
test_dir = 'data/test'

# Define the test size and validation split size
test_size = 0.2
val_split_size = 0.25

# Get a list of all the image filenames and labels
back_filenames = os.listdir(os.path.join(data_dir, 'lyingOnBack'))
belly_filenames = os.listdir(os.path.join(data_dir, 'lyingOnBelly'))
side_filenames = os.listdir(os.path.join(data_dir, 'lyingOnSide'))
sitting_filenames = os.listdir(os.path.join(data_dir, 'sittingDown'))

back_labels = ['back'] * len(back_filenames)
belly_labels = ['belly'] * len(belly_filenames)
side_labels = ['side'] * len(side_filenames)
sitting_labels = ['sitting'] * len(sitting_filenames)

filenames = back_filenames + belly_filenames + side_filenames + sitting_filenames
labels = back_labels + belly_labels + side_labels + sitting_labels

# Split the dataset into training and test sets
train_filenames, test_filenames, train_labels, test_labels = train_test_split(filenames, labels, test_size=test_size, stratify=labels)

# Split the training set into training and validation sets
train_filenames, val_filenames, train_labels, val_labels = train_test_split(train_filenames, train_labels, test_size=val_split_size, stratify=train_labels)

# Create the subdirectories for each class in the training, validation, and test directories
for label in set(train_labels):
    os.makedirs(os.path.join(train_dir, label), exist_ok=True)
    os.makedirs(os.path.join(val_dir, label), exist_ok=True)
    os.makedirs(os.path.join(test_dir, label), exist_ok=True)

# Copy the images to the corresponding subdirectories in the training, validation, and test directories
for filename, label in zip(train_filenames, train_labels):
    if filename in back_filenames:
        src = os.path.join(data_dir, 'lyingOnBack', filename)
    elif filename in belly_filenames:
        src = os.path.join(data_dir, 'lyingOnBelly', filename)
    elif filename in side_filenames:
        src = os.path.join(data_dir, 'lyingOnSide', filename)
    else:
        src = os.path.join(data_dir, 'sittingDown', filename)
    dst = os.path.join(train_dir, label, filename)
    shutil.copy(src, dst)
    
for filename, label in zip(val_filenames, val_labels):
    if filename in back_filenames:
        src = os.path.join(data_dir, 'lyingOnBack', filename)
    elif filename in belly_filenames:
        src = os.path.join(data_dir, 'lyingOnBelly', filename)
    elif filename in side_filenames:
        src = os.path.join(data_dir, 'lyingOnSide', filename)
    else:
        src = os.path.join(data_dir, 'sittingDown', filename)
    dst = os.path.join(val_dir, label, filename)
    shutil.copy(src, dst)
    
for filename, label in zip(test_filenames, test_labels):
    if filename in back_filenames:
        src = os.path.join(data_dir, 'lyingOnBack', filename)
    elif filename in belly_filenames:
        src = os.path.join(data_dir, 'lyingOnBelly', filename)
    elif filename in side_filenames:
        src = os.path.join(data_dir, 'lyingOnSide', filename)
    else:
        src = os.path.join(data_dir, 'sittingDown', filename)
    dst = os.path.join(test_dir, label, filename)
    shutil.copy(src, dst)