# -*- coding: utf-8 -*-

import numpy as np
import chainer
import chainer.functions as F
import chainer.links as L
from chainer import Chain
from libact.base.interfaces import ProbabilisticModel


class MyModel(Chain):
    def __init__(self, unit):
        super(MyModel, self).__init__(
            fc1=L.Linear(None, unit),
            fc2=L.Linear(None, unit),
            fc3=L.Linear(None, 2)
        )
        self.train = True

        
    def __call__(self, x, t):
        h = F.dropout(F.relu(self.fc1(x)), train=self.train, ratio=0.5)
        h = F.dropout(F.relu(self.fc2(h)), train=self.train, ratio=0.5)
        h = self.fc3(h)
        loss = F.softmax_cross_entropy(h, t)
        chainer.report({'loss': loss, 'accuracy': F.accuracy(h, t)}, self)
        return loss


    def extract(self, x):
        h = F.dropout(F.relu(self.fc1(x)), train=False, ratio=0.5)
        return F.dropout(F.relu(self.fc2(h)), train=self.train, ratio=0.5)


    def forward(self, x):
        h = F.dropout(F.relu(self.fc1(x)), train=False, ratio=0.5)
        h = F.dropout(F.relu(self.fc2(h)), train=False, ratio=0.5)
        h = self.fc3(h)
        return F.softmax(h)


class Chainer2Sklearn(ProbabilisticModel):
    def __init__(self, model):
        self.model_ = model

    def train(self, x_data):
        return self.model_

    def predict(self, x_data):
        return self.predict_proba(x_data).argmax(1)

    def score(self, x_data):
        return

    def extract(self, x_data):
        X = np.array(x_data, dtype=np.float32)
        y = self.model_.extract(X)
        return y.data

    def predict_proba(self, x_data):
        X = np.array(x_data, dtype=np.float32)
        y = self.model_.forward(X)
        return y.data
