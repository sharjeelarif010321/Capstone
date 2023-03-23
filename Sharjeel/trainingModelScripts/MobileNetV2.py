import os
import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator

# Set dataset directories and parameters
train_dir = 'dataset/train'
val_dir = 'dataset/val'
test_dir = 'dataset/test'
img_height = 512
img_width = 512
batch_size = 64
num_classes = 4
epochs = 10

# Create ImageDataGenerators for training, validation, and test data
train_datagen = ImageDataGenerator(rescale=1./255)
train_generator = train_datagen.flow_from_directory(
    train_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical')

val_datagen = ImageDataGenerator(rescale=1./255)
validation_generator = val_datagen.flow_from_directory(
    val_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical')

test_datagen = ImageDataGenerator(rescale=1./255)
test_generator = test_datagen.flow_from_directory(
    test_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical',
    shuffle=False)

# Load pre-trained MobileNetV2 with ImageNet weights
base_model = tf.keras.applications.MobileNetV2(input_shape=(img_height, img_width, 3),
                                               include_top=False,
                                               weights='imagenet')

# Set the base model to be non-trainable
base_model.trainable = False

# Create a custom model with MobileNetV2 as the base model
model = tf.keras.Sequential([
    base_model,
    tf.keras.layers.GlobalAveragePooling2D(),
    tf.keras.layers.Dense(1024, activation='relu'),  # Added Dense layer
    tf.keras.layers.BatchNormalization(),            # Added BatchNormalization layer
    tf.keras.layers.Dropout(0.5),                     # Added Dropout layer
    tf.keras.layers.Dense(512, activation='relu'),   # Added another Dense layer
    tf.keras.layers.BatchNormalization(),            # Added another BatchNormalization layer
    tf.keras.layers.Dropout(0.5),                     # Added another Dropout layer
    tf.keras.layers.Dense(num_classes, activation='softmax')
])

# Compile the model
model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
              loss='categorical_crossentropy',
              metrics=['accuracy'])

# Train the model
history = model.fit(train_generator,
                    epochs=epochs,
                    validation_data=validation_generator)

# Save the trained model
model.save('mobilenetv2_modelV14.h5')

# Evaluate the model on the test dataset
loss, accuracy = model.evaluate(test_generator)
print(f"Test loss: {loss}, Test accuracy: {accuracy}")
