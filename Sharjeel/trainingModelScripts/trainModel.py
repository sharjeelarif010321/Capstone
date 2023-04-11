# This code uses TensorFlow and Keras to build and train a deep learning model for image classification. It loads a pre-trained MobileNetV2 model and adds custom layers. The model is compiled and trained on the training data, validated on the validation data, and evaluated on the test data. The PlotCallback class creates a plot of the accuracy and loss of the model during training. The ReduceLROnPlateau and EarlyStopping callbacks are used to adjust the learning rate and stop training early if the validation loss does not improve. The final trained model is saved to a file.

###################################
# Author: Sharjeel Arif
# SID: 200388331
# 08th March 2023
###################################

# REVISION HISTORY

# 08 MAR 23: Created file to train machine learning model

# 08 MAR 23: Followed steps of setting up training model on deeplizard

# 09 MAR 23: Was able to train first model 
# Model was scratch. First model did not use transfer learning

# 11 Mar 23: Consulted Dr. Bais regarding accuracy of a trained model
# Suggested I use transfer learning on a pre-existing model 
# Rather than training a model from scratch

# 12 MAR 23: Implemented changes to import VGG19 Model and use transfer learning to train

# 15 MAR 23: Implemented changes to import VGG16 model and use transfer learning to train

# 20 MAR 23: Implemented changes to import MobileNetV2 model and use transfer learning to train

# 21 MAR 23: Trained first few iterations off 600 size dataset. Inaccurate

# 23 MAR 23: Consulted Dr. Bais to increase accuracy of model
# Was advised to increase dataset

# 29 Mar 23: Increased dataset using augmentation 

# 30 Mar 23: Trained new MobileNetV2 model

# 30 Mar 23: Tested Model, Adjusted Parameters (Added dense and dropout layers and unfroze layers), Trained another

# 30 Mar 23: Tested Model, Adjusted Parameters (Learning Rate), Trained another

# 30 Mar 23: Tested Model, Adjusted Parameters (Adjusted frozen layers), Trained another

# 31 Mar 23: Tested Model, Adjusted Parameters (Adjusted frozen layers), Trained another


import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.layers import Dense, Dropout
from tensorflow.keras.models import Model
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.applications.mobilenet_v2 import MobileNetV2
from tensorflow.keras.callbacks import ReduceLROnPlateau, EarlyStopping
import matplotlib.pyplot as plt
import numpy as np

class PlotCallback(tf.keras.callbacks.Callback):
    def on_train_begin(self, logs=None):
        self.acc = []
        self.val_acc = []
        self.loss = []
        self.val_loss = []

    def on_epoch_end(self, epoch, logs=None):
        self.acc.append(logs.get('accuracy'))
        self.val_acc.append(logs.get('val_accuracy'))
        self.loss.append(logs.get('loss'))
        self.val_loss.append(logs.get('val_loss'))

        plt.figure(figsize=(10, 5))

        plt.subplot(1, 2, 1)
        plt.plot(np.arange(epoch + 1), self.acc, label='Train')
        plt.plot(np.arange(epoch + 1), self.val_acc, label='Validation')
        plt.title('Model accuracy')
        plt.ylabel('Accuracy')
        plt.xlabel('Epoch')
        plt.legend()
        plt.grid(True)

        plt.subplot(1, 2, 2)
        plt.plot(np.arange(epoch + 1), self.loss, label='Train')
        plt.plot(np.arange(epoch + 1), self.val_loss, label='Validation')
        plt.title('Model loss')
        plt.ylabel('Loss')
        plt.xlabel('Epoch')
        plt.legend()
        plt.grid(True)

        plt.tight_layout()
        plt.show(block=False)
        plt.pause(1)
        plt.close()

    def save_plot(self, file_name='training_plot4.png'):
        plt.figure(figsize=(10, 5))

        plt.subplot(1, 2, 1)
        plt.plot(self.acc, label='Train')
        plt.plot(self.val_acc, label='Validation')
        plt.title('Model accuracy')
        plt.ylabel('Accuracy')
        plt.xlabel('Epoch')
        plt.legend()
        plt.grid(True)

        plt.subplot(1, 2, 2)
        plt.plot(self.loss, label='Train')
        plt.plot(self.val_loss, label='Validation')
        plt.title('Model loss')
        plt.ylabel('Loss')
        plt.xlabel('Epoch')
        plt.legend()
        plt.grid(True)

        plt.tight_layout()
        plt.savefig(file_name)
        plt.close()


# Define the paths to the training, validation, and test directories
train_dir = 'dataset/train'
val_dir = 'dataset/val'
test_dir = 'dataset/test'

# Define the image size and batch size
img_size = (224, 224)
batch_size = 16

# Define the number of output classes
num_classes = 4

# Define the MobileNetV2 model with pre-trained weights from ImageNet
base_model = MobileNetV2(input_shape=(img_size[0], img_size[1], 3), include_top=False, weights='imagenet')

# Freeze all layers in the base model
for layer in base_model.layers:
    layer.trainable = False

# Unfreeze the last 30 layers
for layer in base_model.layers[-35:]:
    layer.trainable = True

# Add custom layers on top of the base model for your specific use case
x = base_model.output
x = tf.keras.layers.GlobalAveragePooling2D()(x)
x = Dense(128, activation='relu')(x)
x = Dropout(0.5)(x)
x = Dense(64, activation='relu')(x)  # Additional Dense layer
x = Dropout(0.5)(x)  # Additional Dropout layer
x = Dense(32, activation='relu')(x)  # New dense layer
x = Dropout(0.5)(x)  # New dropout layer
predictions = Dense(num_classes, activation='softmax')(x)

# Create the model
model = Model(inputs=base_model.input, outputs=predictions)

# Compile the model with the desired optimizer, loss function, and metrics
model.compile(optimizer=Adam(lr=1e-4), loss='categorical_crossentropy', metrics=['accuracy'])

# Print a summary of the model architecture
model.summary()

# Define the data generators for training, validation, and test sets
datagen = ImageDataGenerator(
    rescale=1./255,
    validation_split=0.2
)

train_generator = datagen.flow_from_directory(
    train_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical',
    subset='training'
)

val_generator = datagen.flow_from_directory(
    val_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical',
    subset='validation'
)

test_generator = datagen.flow_from_directory(
    test_dir,
    target_size=img_size,
    batch_size=batch_size,
    class_mode='categorical'
)

# Define the ReduceLROnPlateau and EarlyStopping callbacks
reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.2, patience=3, min_lr=1e-6)
early_stopping = EarlyStopping(monitor='val_loss', patience=5, restore_best_weights=True)

# Train the model on the training data, and validate it on the validation data
plot_callback = PlotCallback()

history = model.fit(
    train_generator,
    epochs=30,
    validation_data=val_generator,
    callbacks=[reduce_lr, early_stopping, plot_callback]
)

# Save final plot
plot_callback.save_plot('final_training_plot4.png')

# Evaluate the model on the test data
test_loss, test_acc = model.evaluate(test_generator)
print('Test accuracy:', test_acc)

# Save the trained model to a file (optional)
model.save('baby_posture_modelv4.h5')
