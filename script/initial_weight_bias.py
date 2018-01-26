# -*- coding: utf-8 -*-

import cPickle as pickle
import numpy as np


model_path = "../bin/data/vggface/VGG_FACE.pkl"
model = pickle.load(open(model_path, 'rb'))

np.save("initial_W.npy", model.fc7.W.data)
np.save("initial_b.npy", model.fc7.b.data)