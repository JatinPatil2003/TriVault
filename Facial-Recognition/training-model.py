import cv2
import numpy as np
from os import listdir
from os.path import isfile, join

data_path = 'C:/Users/jatin/Documents/fac/Images/'
onlyfiles = [f for f in listdir(data_path) if isfile(join(data_path,f))]

Training_Data, Lables = [], []

for i, files in enumerate(onlyfiles):
    image_path = data_path + onlyfiles[i]
    images = cv2.imread(image_path,cv2.IMREAD_GRAYSCALE)
    Training_Data.append(np.asarray(images,dtype=np.uint8))
    Lables.append(i)

Lables = np.asarray(Lables, dtype=np.int32)

model = cv2.face.LBPHFaceRecognizer_create()

model.train(np.asarray(Training_Data),np.asarray(Lables))

print("Model Training Complete !!")
