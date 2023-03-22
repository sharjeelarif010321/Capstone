import os
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.applications import VGG19

# Set the seed value for reproducibility
np.random.seed(42)
tf.random.set_seed(42)

# Set the directories for the training, validation, and testing data
train_dir = 'data/train'
val_dir = 'data/val'
test_dir = 'data/test'

# Set the image size and batch size
img_size = (512, 512)
batch_size = 16

# Load the VGG19 model
vgg19 = VGG19(
    include_top=False,
    weights='imagenet',
    input_shape=(img_size[0], img_size[1], 3)
)

# Freeze the layers in the pre-trained model
for layer in vgg19.layers:
    layer.trainable = False

# Create a new model and add the VGG19 model as a layer
model = Sequential()
model.add(vgg19)

# Add new layers
model.add(Flatten())
model.add(Dense(256, activation='relu'))
model.add(Dense(4, activation='softmax'))

# Compile the model
model.compile(
    optimizer='adam',
    loss='categorical_crossentropy',
    metrics=['accuracy']
)

# Set up the data generators for the training, validation, and testing data
train_datagen = ImageDataGenerator(
    rescale=1./255,
    rotation_range=30,
    width_shift_range=0.1,
    zoom_range=0.05,
    #preprocessing_function=lambda x: np.stack((x[..., 0],)*3, axis=-1) # convert to grayscale
)

train_generator = train_datagen.flow_from_directory(
    train_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical'
)

valid_datagen = ImageDataGenerator(rescale=1./255)

valid_generator = valid_datagen.flow_from_directory(
    val_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical'
)

test_datagen = ImageDataGenerator(rescale=1./255)

test_generator = test_datagen.flow_from_directory(
    test_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical'
)

# Calculate the step sizes for the training, validation, and testing generators
STEP_SIZE_TRAIN = train_generator.n // train_generator.batch_size
STEP_SIZE_VALID = valid_generator.n // valid_generator.batch_size
STEP_SIZE_TEST = test_generator.n // test_generator.batch_size

# Train the model
history = model.fit(
    train_generator,
    steps_per_epoch=STEP_SIZE_TRAIN,
    epochs=8,
    validation_data=valid_generator,
    validation_steps=STEP_SIZE_VALID
)

# Save the trained model to disk
model.save('my_model_vgg19v11.h5')

# Evaluate the model on the testing data
test_loss, test_acc = model.evaluate(test_generator, steps=STEP_SIZE_TEST)
print('Test loss:', test_loss)
print('Test accuracy:', test_acc)
