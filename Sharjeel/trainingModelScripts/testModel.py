import tensorflow as tf
import numpy as np
import os
from PIL import Image
from keras.preprocessing import image
from tensorflow.keras.preprocessing.image import load_img

# Load the model
model = tf.keras.models.load_model("my_model_vgg19v6.h5")

# Define the image size
img_size = (512, 512)

# Get the test directory path and classes
test_dir = "data/test"
test_classes = os.listdir(test_dir)

# Loop through each subdirectory in the test directory and evaluate the model
for sub_dir in test_classes:
    print(f"Evaluating images in {sub_dir} directory...")
    sub_dir_path = os.path.join(test_dir, sub_dir)
    for image_file in os.listdir(sub_dir_path):
        image_path = os.path.join(sub_dir_path, image_file)
        image = Image.open(image_path).resize(img_size)
        image = np.array(image)
        image = np.expand_dims(image, axis=0)
        result = model.predict(image)
        if result[0][0] > result[0][1] and result[0][0] > result[0][2] and result[0][0] > result[0][3]:
            print(f"{image_file} is lying on its back")
        elif result[0][1] > result[0][0] and result[0][1] > result[0][2] and result[0][1] > result[0][3]:
            print(f"{image_file} is lying on its belly")
        elif result[0][2] > result[0][0] and result[0][2] > result[0][1] and result[0][2] > result[0][3]:
            print(f"{image_file} is lying on its side")
        else:
            print(f"{image_file} is sitting down")
