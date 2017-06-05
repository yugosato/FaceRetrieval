# -*- coding: utf-8 -*-

home_dir = "/home/yugo/workspace/Interface/trainer"

import chainer
import numpy as np
from chainer import training
from chainer.training import extensions
import gc
import sys
import os
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'

from dataset import ImportDataset
import mymodel


# File paths
listfile = "/home/yugo/workspace/Interface/bin/log/feedback.txt"
inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-vgg.npy"

# Training parameter
epoch = 1
batch_size = 5
gpu_id = 0

# Initialize model to train
model = mymodel.MyModel()

if gpu_id >= 0:
    chainer.cuda.get_device_from_id(gpu_id).use()
    model.to_gpu()

# Load datasets and set up iterator
train = ImportDataset(listfile, inputfile)
train_iter = chainer.iterators.SerialIterator(train, batch_size=batch_size)

# Set optimizer
optimizer = chainer.optimizers.AdaDelta()
optimizer.setup(model)

# # Copy chain with shared parameters to flip 'train' flag only in test
# eval_model = model.copy()
# eval_model.train = False

# Set up updateer and trainer
updater = training.StandardUpdater(train_iter, optimizer, device=gpu_id)
trainer = training.Trainer(updater, (epoch, "epoch"), os.path.join(home_dir, "result"))
trainer.extend(extensions.LogReport())
# trainer.extend(extensions.PrintReport(["epoch", "main/loss", "main/accuracy"]))
trainer.extend(extensions.PlotReport(["main/loss"], "epoch", file_name="loss.png"))
trainer.extend(extensions.PlotReport(["main/accuracy"], "epoch", file_name="accuracy.png"))

# Run trainer
trainer.run()

model.to_cpu();
weight = model.fc2.W.data
bias = model.fc2.b.data

print "finished"

