# -*- coding: utf-8 -*-

import chainer
from chainer import Chain
import chainer.functions as F
import chainer.links as L


class MyModel(Chain):

    def __init__(self):
        super(MyModel, self).__init__(
            fc1=L.Linear(None, 4096),
            fc2=L.Linear(None, 1024),
            fc3=L.Linear(None, 2)
        )
        self.train = True

        
    def __call__(self, x, t):
        h = F.relu(self.fc1(x))
        h = F.relu(self.fc2(h))
        h = self.fc3(h)
        
        loss = F.softmax_cross_entropy(h, t)
        chainer.report({'loss': loss, 'accuracy': F.accuracy(h, t)}, self)
        return loss