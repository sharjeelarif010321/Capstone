# This code converts a Keras model (.h5) to a TensorFlow Lite model (.tflite). It loads the Keras model using tf.keras.models.load_model(), creates a converter using tf.lite.TFLiteConverter.from_keras_model(), and converts the Keras model to a TensorFlow Lite model using converter.convert(). The code saves the TensorFlow Lite model to a file using the open() function and prints a message indicating that the conversion is complete.

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 08th March 2023
###################################

# REVISION HISTORY

# 28 Mar 23: Created this file to convert standart h5 model to tflite to be able to work effeciently on the raspberry pi 4 model b


import tensorflow as tf

# Load the .h5 model
h5_model_path = 'baby_posture_modelv4.h5'
model = tf.keras.models.load_model(h5_model_path)

# Convert the model to .tflite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the .tflite model
tflite_model_path = 'baby_posture_modelv4.tflite'
with open(tflite_model_path, 'wb') as f:
    f.write(tflite_model)

print('Model converted and saved as', tflite_model_path)
